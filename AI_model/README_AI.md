# EAE AI Model — Usage
--------------------------------
# ONLY DO THIS IF YOU ARE GOING TO GENERATE A NEW MODEL

1. Configure CreateData.py as needed.
2. Use CreateData.py to create the training data for the AI model. (Port must be connected to the ESP32)
3. Open TrainModel.ipynb to train your model. (VSCode/Google Colab)
4. Save the trained model as .pkl for testing in Python.
5. Convert the trained model to C++ using converter.py which will convert it into a C file - ESP32 memory constraints will be automatically patched

# END RESULT:
- AIEngine will use the AI predictions as primary, then fallback modules handle edge cases.