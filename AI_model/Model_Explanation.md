# EAE AI Model

This AI model is designed to predict actions for EAE based on sensor readings and fallback predictions:
- Temperature (temp)
- Humidity (hum)
- Light intensity (light)
- Motion input (motion)
- Predictor outputs: predTemp, predHum, predLight
- Anomaly Detector outputs: anomalyTemp, anomalyHum, anomalyLight

The AI model is the primary decision maker. If AI fails, the existing fallback which uses statistics and mathematical calculations (Predictor + AnomalyDetector + ActivityRecognizer) takes over and produces the output.