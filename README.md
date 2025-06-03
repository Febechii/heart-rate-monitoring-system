# Heart Rate and Pain Monitoring System (PPG + ESP32)

This project is a real-time **heart rate and post-operative pain monitoring system** using a PPG (Photoplethysmography) sensor and the ESP32 microcontroller. It extracts heart rate, AC/DC ratio, and HRV metrics to classify pain levels into categories like **"No Pain"**, **"Mild Pain"**, and **"High Pain"**, based on thresholds derived from clinical research.

Developed as part of my **BEng (Honours) Mechatronics Engineering** final year project at **TU Dublin**, the system is low-cost, portable, and suitable for clinical and home-care environments.

---

## 🔧 Key Features

- 📟 Real-time heart rate (BPM) display via OLED and web dashboard  
- 📈 PPG signal filtering using Butterworth bandpass, low-pass, and high-pass filters  
- 🧠 Pain level classification using:
  - **AC/DC ratio threshold** (1.33% cutoff)
  - **LF/HF ratio** from HRV analysis  
- 📊 Frequency domain analysis using Fast Fourier Transform (FFT)  
- 🌐 Wi-Fi-based WebSocket dashboard for live metrics  
- 🔁 Sliding window averaging for trend smoothing  

---

## 📐 System Architecture

**Hardware:**
- ESP32 microcontroller  
- XD-58C PPG pulse sensor  
- 0.96" OLED display (SSD1306)  
- Optional: Wi-Fi access point for dashboard

**Software Stack:**
- Arduino C++ (ESP32)
- HTML/CSS/JavaScript (for the dashboard)
- WebSocket for real-time comms
- FFT and filtering implemented on-device

---

## 💡 Pain Classification Logic

| Metric        | Threshold / Range         | Pain Level  |
|---------------|----------------------------|-------------|
| AC/DC Ratio   | > 2.0%                     | No Pain     |
| AC/DC Ratio   | 1.0% – 2.0%                | Mild Pain   |
| AC/DC Ratio   | ≤ 1.0%                     | High Pain   |
| LF/HF Ratio   | ↑ (> baseline)             | Likely pain |
| RMSSD / SDNN  | ↓ (low variability)        | Likely pain |

Thresholds based on: Peng et al., 2014; Hyunjun et al., 2019

---

## 🖥️ Web Dashboard

Real-time web interface displays:
- BPM
- AC/DC ratio
- LF/HF ratio
- Raw/filtered waveform
- Live pain classification status
![image](https://github.com/user-attachments/assets/34478839-bff2-4eaf-8784-dfa4c17fb520)
![image](https://github.com/user-attachments/assets/aaaf58f5-cfa5-4119-b3da-ea0b6b152ce6)
![image](https://github.com/user-attachments/assets/a737497b-8d1a-45e3-9cdf-dcbda18c2675)
![image](https://github.com/user-attachments/assets/fc642708-a984-4fe5-b152-863cab50ba25)
![image](https://github.com/user-attachments/assets/3c1eca56-bcec-49cf-8142-d957c67c4c5b)
![image](https://github.com/user-attachments/assets/14132def-8ea0-4f0b-ad71-04db989c2cff)

## 📁 Project Structure



---

## 🚀 Getting Started

### Upload to ESP32
1. Open `main.ino` in Arduino IDE
2. Connect ESP32 board
3. Select correct COM port and board model
4. Upload the sketch

### View the Dashboard
- Connect to the ESP32's Wi-Fi or local network IP
- Open the `index.html` file in your browser or serve via ESP32

---

## 🧪 Future Improvements
- Add SD card or SPIFFS logging
- Replace rule-based logic with machine learning
- Add GSR or EMG sensors for multimodal detection

---

## 👤 Author

**Peculiar Uzoatuegwu**  
BEng (Hons) Mechatronics Engineering, TU Dublin  
[LinkedIn](https://www.linkedin.com/in/peculiaru)  

---

## 📜 License

MIT – Open for academic or personal use.
