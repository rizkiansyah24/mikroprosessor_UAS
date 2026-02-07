# ESP32 Smart Control Pompa Air

Sistem kontrol pompa air berbasis **ESP32**, **Web Server**, dan **MQTT** yang dapat dikendalikan melalui browser, tombol fisik, dan cloud MQTT.

Proyek ini dibuat sebagai bagian dari **Ujian Akhir Semester (UAS)**.

Link Demo video youtube : https://youtu.be/5GrOgeQ3584

## 📌 Fitur Utama

* ✅ Kontrol pompa melalui web browser
* ✅ Kontrol manual menggunakan tombol
* ✅ Integrasi MQTT (Cloud Shiftr Public)
* ✅ Konfigurasi WiFi melalui web
* ✅ Penyimpanan WiFi otomatis (Preferences)
* ✅ Monitoring status real-time
* ✅ Mode Access Point otomatis

---

## 🛠️ Teknologi yang Digunakan

### Hardware

* ESP32
* ARDUINO IDE
* Laptop

### Software & Library

* Arduino IDE
* WiFi.h
* WebServer.h
* Preferences.h
* ArduinoJson.h
* PubSubClient.h

---

## 📂 Struktur Proyek

```
ESP32-Smart-Pompa/
├── src/
│   └── main.ino
├── README.md
└── docs/
    └── laporan.pdf
```

---

## ⚙️ Cara Kerja Sistem

1. ESP32 membaca data WiFi dari memori.
2. Jika belum ada WiFi, ESP32 masuk AP Mode.
3. User mengatur WiFi melalui web.
4. ESP32 terhubung ke internet.
5. ESP32 terhubung ke MQTT Broker.
6. Pompa dapat dikontrol melalui:

   * Website
   * MQTT
   * Tombol
7. Status pompa dikirim secara real-time.

---

## 🚀 Instalasi & Penggunaan

### 1. Clone Repository

```bash
git clone https://github.com/username/ESP32-Smart-Pompa.git
```

### 2. Install Library

Pastikan library berikut sudah terinstall di Arduino IDE:

* PubSubClient
* ArduinoJson
* ESP32 Board Package

---

### 3. Upload Program

1. Hubungkan ESP32 ke komputer
2. Buka file `main.ino`
3. Pilih board: **ESP32 Dev Module**
4. Klik Upload

---

### 4. Konfigurasi WiFi

Jika belum pernah terhubung ke WiFi:

1. Sambungkan HP/Laptop ke WiFi:

   ```
   ESP32-Config
   ```
2. Buka browser:

   ```
   192.168.4.1
   ```
3. Masukkan SSID dan Password
4. Klik **Simpan WiFi**

ESP32 akan restart dan terhubung otomatis.

---

## 🌐 Akses Web Control

Jika ESP32 sudah terhubung ke WiFi:

1. Lihat IP Address di Serial Monitor
2. Buka browser
3. Masukkan IP tersebut

Contoh:

```
http://192.168.1.10
```

---

## 📡 Konfigurasi MQTT

Broker:

```
public.cloud.shiftr.io
```

Port:

```
1883
```

Topic:

```
iot/esp32/pompa/control
iot/esp32/pompa/status
```

Format Pesan:

```
ON  → Menyalakan Pompa
OFF → Mematikan Pompa
```

---

## 🧪 Pengujian

Pengujian dilakukan dengan:

* Mengontrol dari web
* Mengontrol dari MQTT client
* Menekan tombol fisik
* Mematikan WiFi (AP Mode)

Hasil: Sistem berjalan stabil dan responsif.

---

## 📈 Pengembangan Selanjutnya

* 🔐 Autentikasi login
* 📱 Aplikasi Android
* 🔒 MQTT SSL
* 💧 Sensor level air
* ☁️ Dashboard cloud

---

## 👤 Author

Nama : Aldi Rizkiansyah
Proyek : UAS IoT ESP32
Tahun : 2026

---

## 📄 Lisensi

Proyek ini dibuat untuk keperluan pembelajaran dan akademik.

Bebas digunakan untuk pengembangan non-komersial.

---

## ⭐ Dukungan

Jika proyek ini membantu, silakan beri ⭐ pada repository ini.

Terima kasih 🙏
