# AI_model/constraints_patch.py

import re
import os
import sys

def apply_constraints_patch(c_file_path):
    # check if file exists
    if not os.path.exists(c_file_path):
        print(f"[ERROR] {c_file_path} not found")
        return

    with open(c_file_path, "r") as f:
        code = f.read()

    pattern = re.compile(r"\s*double\s+(var\d+)\s*\[(\d+)\]\s*;")
    matches = pattern.findall(code)

    if not matches:
        print("[INFO] No stack arrays found")
        return

    print(f"[INFO] Found {len(matches)} local arrays")

    code = pattern.sub("", code)

    # deduplicate and prepare globals
    seen = set()
    globals_code = "// ESP32 stack fix (deduplicated)\n"
    for name, size in matches:
        if name in seen:
            continue
        seen.add(name)
        globals_code += f"static double {name}[{size}];\n"
    globals_code += "\n"

    # insert globals at the top after includes
    lines = code.splitlines()
    insert_idx = 0
    for i, line in enumerate(lines):
        if not line.startswith("#include"):
            insert_idx = i
            break

    lines.insert(insert_idx, globals_code)
    code = "\n".join(lines)

    with open(c_file_path, "w") as f:
        f.write(code)

    print(f"[SUCCESS] {len(seen)} unique stack arrays moved to globals")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python constraints_patch.py path/to/model.c")
        sys.exit(1)
    apply_constraints_patch(sys.argv[1])
