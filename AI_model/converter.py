# AI_model/converter.py

import joblib
import m2cgen as m2c
import os
import re
import subprocess

# path setup
ai_folder = os.path.dirname(__file__)
project_src = os.path.join(ai_folder, "..", "src")
os.makedirs(project_src, exist_ok=True)

model_path = os.path.join(ai_folder, "model.pkl")
if not os.path.exists(model_path):
    raise FileNotFoundError(f"{model_path} not found. Train the model first.")

clf = joblib.load(model_path)
print("[INFO] Loaded model.pkl")

# helper functions
def split_helpers_and_score(code: str):
    helper_pattern = (
        r"(void\s+add_vectors[\s\S]*?^\})"
        r"|(void\s+mul_vector_number[\s\S]*?^\})"
    )

    helpers = re.findall(helper_pattern, code, re.MULTILINE)
    helpers_code = "\n".join(h for tup in helpers for h in tup if h)

    code_no_helpers = re.sub(
        helper_pattern, "", code, flags=re.MULTILINE
    ).strip()

    return helpers_code, code_no_helpers

# generate C code
tags = ["led1", "led2", "buzz"]
codes = [m2c.export_to_c(est, function_name=f"score_{tag}") for est, tag in zip(clf.estimators_, tags)]

# extract helpers and score functions
helpers_code, score_led1 = split_helpers_and_score(codes[0])
score_led2 = split_helpers_and_score(codes[1])[1]
score_buzz = split_helpers_and_score(codes[2])[1]

# model.h content
header = """#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void score_led1(double * input, double * output);
void score_led2(double * input, double * output);
void score_buzz(double * input, double * output);

int runAIModel(
    float temp, float hum, int light, int motion,
    float predTemp, float predHum, int predLight,
    float anomalyTemp, float anomalyHum, float anomalyLight
);

#ifdef __cplusplus
}
#endif
"""

# source code content
source = f"""
#include "model.h"

{helpers_code}

{score_led1}

{score_led2}

{score_buzz}

int runAIModel(
    float temp, float hum, int light, int motion,
    float predTemp, float predHum, int predLight,
    float anomalyTemp, float anomalyHum, float anomalyLight
) {{
    double input[10] = {{
        (double)temp, (double)hum, (double)light, (double)motion,
        (double)predTemp, (double)predHum, (double)predLight,
        (double)anomalyTemp, (double)anomalyHum, (double)anomalyLight
    }};

    double output[1];
    int mask = 0;
    score_led1(input, output);
    if (output[0] <= 0.5) mask |= 0x01;
    score_led2(input, output);
    if (output[0] <= 0.5) mask |= 0x02;
    score_buzz(input, output);
    if (output[0] <= 0.5) mask |= 0x04;

    return mask;
}}
"""

# safeguard checks
if re.search(r'\bvoid\s+score\s*\$', source):
    raise RuntimeError("UN-NAMESPACED score() STILL EXISTS")

# write
with open(os.path.join(project_src, "model.h"), "w") as f:
    f.write(header)

with open(os.path.join(project_src, "model.c"), "w") as f:
    f.write(source)

print(f"[OK] model.c / model.h generated in {project_src}")

# implement constraints patch
patch_script = os.path.join(ai_folder, "constraints_patch.py")
model_c_path = os.path.join(project_src, "model.c")
subprocess.run(["python", patch_script, model_c_path])