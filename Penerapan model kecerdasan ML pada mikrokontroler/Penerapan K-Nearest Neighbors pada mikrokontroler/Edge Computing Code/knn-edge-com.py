import requests
import joblib
import numpy as np
from flask import Flask, jsonify

# ================= Konfigurasi =================
ESP32_URL = "http://192.168.28.23/data.json"  # ganti IP ESP32
MODEL_PATH = "knn_model.pkl"
#SCALER_PATH = "scaler.pkl"  # hapus jika tidak pakai scaler

# ================= Load Model =================
knn_model = joblib.load(MODEL_PATH)

try:
    scaler = joblib.load(SCALER_PATH)
    use_scaler = True
except:
    use_scaler = False

# ================= Flask =================
app = Flask(__name__)

# ================= Endpoint =================
@app.route("/predict", methods=["GET"])
def predict():

    # --- Ambil data dari ESP32 ---
    response = requests.get(ESP32_URL, timeout=5)
    data = response.json()

    # --- Ambil 8 fitur ---
    X = np.array([[
        data["temperature"],
        data["humidity"],
        data["lux"],
        data["suara"],
        data["co2"],
        data["panjang"],
        data["lebar"],
        data["weather_code"]
    ]])

    # --- Scaling (jika ada) ---
    if use_scaler:
        X = scaler.transform(X)

    # --- Prediksi ---
    prediction = knn_model.predict(X)[0]

    return jsonify({
        "status": "success",
        "input": X.tolist(),
        "prediction": int(prediction)
    })


# ================= Main =================
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=2001, debug=True)
