#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <vector>

// Menggunakan namespace agar kode lebih rapi
using namespace cv;
using namespace cv::dnn;
using namespace std;

// Ini adalah fungsi untuk memformat gambar sebelum masuk ke model
// (Membuat gambar menjadi kotak 640x640)
Mat format_image_for_yolo(const Mat& source) {
    int col = source.cols;
    int row = source.rows;
    int _max = MAX(col, row); // Cari sisi terpanjang
    // Buat "kanvas" hitam persegi
    Mat result = Mat::zeros(_max, _max, CV_8UC3);
    // Salin gambar asli ke pojok kiri atas kanvas
    source.copyTo(result(Rect(0, 0, col, row)));
    return result;
}

int main() {
    // ---- 1. MEMUAT MODEL ----
    // Pastikan "yolov8n.onnx" ada di folder yang sama
    String modelPath = "yolov8n.onnx";
    Net net;

    try {
        net = readNet(modelPath);
    } catch (const cv::Exception& e) {
        cerr << "Error memuat model: " << e.what() << endl;
        return -1;
    }

    // Tentukan target komputasi (CPU)
    net.setPreferableBackend(DNN_BACKEND_OPENCV);
    net.setPreferableTarget(DNN_TARGET_CPU);


    // ---- 2. MEMUAT GAMBAR ----
    // Pastikan Anda punya "gambar.jpg" di folder yang sama
    Mat frame = imread("gambar.jpg");
    if (frame.empty()) {
        cerr << "Error: Tidak bisa membuka gambar!" << endl;
        return -1;
    }

    Mat modelInput = format_image_for_yolo(frame);

    // ---- 3. PERSIAPAN INFERENCE ----
    // Konversi gambar ke format "blob" yang diterima YOLO
    // YOLOv8 menerima input 640x640
    Mat blob;
    blobFromImage(modelInput, blob, 1.0 / 255.0, Size(640, 640), Scalar(), true, false);
    net.setInput(blob);

    // ---- 4. JALANKAN INFERENCE ----
    vector<Mat> outputs;
    // Jalankan model dan ambil hasilnya
    net.forward(outputs, net.getUnconnectedOutLayersNames());
    
    // Output YOLOv8 adalah [84 x 8400]
    // 84 = 4 (box) + 80 (class scores)
    // 8400 = jumlah prediksi kotak
    // Kita ubah bentuknya agar lebih mudah dibaca
    Mat output_buffer = outputs[0].reshape(1, outputs[0].size[1]); // Ubah shape jadi [84, 8400]

    // ---- 5. POST-PROCESSING (Mengurai Hasil) ----
    float conf_threshold = 0.25; // Ambang batas kepercayaan (25%)
    float nms_threshold = 0.45;  // Ambang batas Non-Maximum Suppression

    vector<Rect> boxes;
    vector<float> confidences;
    vector<int> class_ids;

    // Faktor skala untuk mengembalikan koordinat ke gambar asli
    float x_factor = modelInput.cols / 640.0;
    float y_factor = modelInput.rows / 640.0;

    // Kita harus mem-transpose output agar menjadi [8400 x 84]
    // Sehingga kita bisa cek satu per satu dari 8400 deteksi
    Mat detections = output_buffer.t(); // Transpose

    for (int i = 0; i < detections.rows; ++i) {
        Mat detection = detections.row(i);
        
        // Dapatkan skor kelas (kolom ke-4 sampai ke-83)
        Mat classes_scores = detection.colRange(4, 84);
        Point class_id_point;
        double max_class_score;
        // Cari skor kelas tertinggi
        minMaxLoc(classes_scores, 0, &max_class_score, 0, &class_id_point);

        // Cek apakah skor tertinggi itu di atas ambang batas
        if (max_class_score > conf_threshold) {
            float confidence = max_class_score;
            
            // Dapatkan koordinat kotak (cx, cy, w, h)
            float cx = detection.at<float>(0, 0);
            float cy = detection.at<float>(0, 1);
            float w = detection.at<float>(0, 2);
            float h = detection.at<float>(0, 3);

            // Konversi ke (x, y) pojok kiri atas
            int left = (int)((cx - 0.5 * w) * x_factor);
            int top = (int)((cy - 0.5 * h) * y_factor);
            int width = (int)(w * x_factor);
            int height = (int)(h * y_factor);

            boxes.push_back(Rect(left, top, width, height));
            confidences.push_back(confidence);
            class_ids.push_back(class_id_point.x); // class_id_point.x adalah indeks dari kelas
        }
    }

    // Terapkan Non-Maximum Suppression untuk menghilangkan kotak ganda
    vector<int> indices;
    NMSBoxes(boxes, confidences, conf_threshold, nms_threshold, indices);

    // ---- 6. GAMBAR HASIL ----
    for (int idx : indices) {
        Rect box = boxes[idx];
        rectangle(frame, box, Scalar(0, 255, 0), 2);
        
        // Anda bisa menambahkan label kelas jika punya daftar nama kelas
        string label = "Class: " + to_string(class_ids[idx]) + ", Conf: " + format("%.2f", confidences[idx]);
        cout << "Deteksi: " << label << " di " << box << endl;
        putText(frame, label, Point(box.x, box.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
    }

    // Simpan atau tampilkan hasil
    imwrite("hasil_deteksi.jpg", frame);
    cout << "Gambar hasil disimpan di 'hasil_deteksi.jpg'" << endl;

    // // Jika Anda punya desktop, uncomment ini untuk menampilkan
    // imshow("Hasil Deteksi YOLOv8 C++", frame);
    // waitKey(0); 

    return 0;
}
