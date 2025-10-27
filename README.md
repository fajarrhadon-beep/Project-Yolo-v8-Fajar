# Deteksi Objek YOLOv8 C++ dengan OpenCV DNN

Ini adalah proyek C++ sederhana untuk menjalankan deteksi objek YOLOv8 (`.onnx`) menggunakan modul DNN dari OpenCV.

## ⚠️ Peringatan Ketergantungan (Dependencies)

Proyek ini **WAJIB** menggunakan **OpenCV versi 4.6.0 atau lebih baru**. 

Versi OpenCV standar dari repositori `apt` (seperti 4.5.4) **TIDAK AKAN BERFUNGSI** karena terlalu tua untuk membaca model YOLOv8 ONNX. Anda sangat disarankan untuk meng-compile OpenCV 4.9.0 (atau lebih baru) dari source.

### Kebutuhan Utama:
* `build-essential` (compiler `g++`, `make`, dll.)
* `cmake`
* `git`
* **OpenCV 4.9.0** (dibangun dari source)

### Kebutuhan Model:
Proyek ini membutuhkan paket `ultralytics` dari Python **HANYA untuk mengekspor model**.
```bash
pip install ultralytics

Cara Menjalankan Proyek Ini

Ikuti langkah-langkah ini dari awal:

1. Clone Repository:
Bash

git clone https-url-anda
cd nama-folder-proyek-anda

2. Dapatkan Model ONNX: Model .onnx tidak disertakan di repo ini karena ukurannya besar. Anda harus membuatnya sendiri menggunakan perintah yolo:
Bash

yolo export model=yolov8n.pt format=onnx

(Ini akan membuat file yolov8n.onnx di folder Anda).

3. Siapkan Gambar: Letakkan gambar yang ingin Anda tes di folder ini dan beri nama gambar.jpg.

4. Kompilasi Kode C++: Pastikan pkg-config Anda dapat menemukan OpenCV 4.9.0. (Anda mungkin perlu menjalankan export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH terlebih dahulu).
Bash

g++ deteksi.cpp -o program_deteksi $(pkg-config --cflags --libs opencv4)

5. Jalankan Program:
Bash

./program_deteksi

Program akan berjalan dan membuat file hasil_deteksi.jpg di folder yang sama.


---

### Langkah 3: Simpan dan Keluar

1.  Di dalam `nano`, tekan **Ctrl+O** (untuk "Write Out" / Simpan).
2.  Tekan **Enter** untuk mengkonfirmasi nama file (`README.md`).
3.  Tekan **Ctrl+X** untuk keluar dari `nano`.

---

### Langkah 4: Unggah `README.md` ke GitHub

Sekarang, kita kirim file instruksi baru ini ke GitHub.

1.  **Tambahkan file README:**
    ```bash
    git add README.md
    ```

2.  **Commit file tersebut:**
    ```bash
    git commit -m "Menambahkan instruksi di README.md"
    ```

3.  **Push ke GitHub:**
    ```bash
    git push origin main
    ```

**Selesai!** Sekarang, jika Anda me-refresh halaman GitHub proyek Anda, Anda akan me
