# AI_model/CreateData.py

import pandas as pd
import random
import math
from collections import deque

# data generation configuration
MAX_SAMPLES = 2000
WINDOW = 30
PREDICT_AHEAD = 3
OUT_FILE = "training_data.csv"
ZSCORE_THRESHOLD = 3.0
TEMP_THRESHOLD = 35
HUM_THRESHOLD = 80
LIGHT_THRESHOLD = 3100

tbuf = deque(maxlen=WINDOW)
hbuf = deque(maxlen=WINDOW)
lbuf = deque(maxlen=WINDOW)

# helper functions
def zscore(v, buf):
    if len(buf) < 6:
        return 0.0
    m = sum(buf) / len(buf)
    var = sum((x - m) ** 2 for x in buf) / len(buf)
    sd = math.sqrt(var) if var > 1e-4 else 1e-4
    return (v - m) / sd

def predict(buf, ahead):
    return buf[-ahead - 1] if len(buf) >= ahead + 1 else buf[-1]

rows = []

# main data generation loop
for _ in range(MAX_SAMPLES):
    temp = random.uniform(18, 42)
    hum = random.uniform(20, 90)
    light = random.randint(0, 4095)   # higher = darker
    motion = random.choice([0, 1])

    tbuf.append(temp)
    hbuf.append(hum)
    lbuf.append(light)

    if len(tbuf) < WINDOW:
        continue

    rows.append([
        temp,
        hum,
        light,
        motion,
        predict(tbuf, PREDICT_AHEAD),
        predict(hbuf, PREDICT_AHEAD),
        predict(lbuf, PREDICT_AHEAD),
        zscore(temp, tbuf),
        zscore(hum, hbuf),
        zscore(light, lbuf),
        1 if light > LIGHT_THRESHOLD 
            or predict(lbuf, PREDICT_AHEAD) > LIGHT_THRESHOLD 
            or zscore(light, lbuf) > ZSCORE_THRESHOLD else 0,       # LED1 > DARK
        1 if motion else 0,                                         # LED2
        1 if temp > TEMP_THRESHOLD 
            or predict(tbuf, PREDICT_AHEAD) > TEMP_THRESHOLD 
            or zscore(temp, tbuf) > ZSCORE_THRESHOLD else 0         # BUZZ > TEMP ONLY
    ])

#  save to CSV
df = pd.DataFrame(rows, columns=[
    "temp", "hum", "light", "motion",
    "predTemp", "predHum", "predLight",
    "anomTemp", "anomHum", "anomLight",
    "led1", "led2", "buzz"
])

df.to_csv(OUT_FILE, index=False)
print("[OK] training_data.csv generated")