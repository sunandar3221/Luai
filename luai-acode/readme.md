# Luai Language Support & Standalone Runner for Acode (Android)

Plugin resmi **Acode Editor** (Android IDE) untuk bahasa pemrograman **Luai** (dialek Bahasa Indonesia dari Lua 5.4). Dilengkapi dengan **Syntax Highlighting Ace Mode**, **Integrated In-App Runner**, dan **Zero-Config Execution (Berjalan langsung di Android tanpa perlu Termux/Root)**.

---

## Fitur Utama

1. **Zero-Configuration & Siap Pakai di Android**:
   - Engine runtime Luai berbasis JavaScript telah dibundel penuh di dalam plugin ini.
   - Anda **tidak perlu menginstal Termux**, tidak perlu root, dan **tidak perlu menyetel PATH** atau mengunduh binary eksternal di Android.
2. **Tombol Eksekusi Cepat (Floating Action Button ▶️)**:
   - Tombol mengambang hijau **Play ▶️** akan muncul secara otomatis di sudut kanan bawah Acode saat Anda membuka berkas berekstensi `.luai`.
   - Cukup satu sentuhan untuk mengeksekusi kode secara langsung.
3. **Modal Terminal Console Interaktif**:
   - Menampilkan hasil output `cetak(...)` secara rapi dan real-time.
   - Dilengkapi tombol **Salin (Copy)** untuk menyalin seluruh output ke clipboard Android.
   - Dilengkapi tombol **Bersihkan (Clear)** dan **Tutup (Close)**.
   - Menampilkan indikator waktu eksekusi program dalam milidetik (*ms*).
4. **Pewarnaan Sintaks Lengkap (Ace Editor Mode)**:
   - Menyorot 22 kata kunci kontrol Bahasa Indonesia (`jika`, `maka`, `selesai`, `fungsi`, `lokal`, `untuk`, `selama`, dll.).
   - Menyorot modul standar (`matematika`, `tabel`, `teks`, `sistem`, `io`, `korutin`).
   - Menyorot konstanta (`benar`, `salah`, `kosong`, `nihil`), fungsi bawaan, string tunggal/ganda/multiline, komentar (`--`, `--[[ ]]`), serta operator.
5. **Dukungan Command Palette**:
   - `Jalankan Berkas Luai (Run Luai File)`
   - `Buka Terminal Interaktif REPL Luai`

---

## Panduan Instalasi di Android

### Cara 1: Menggunakan Berkas `plugin.zip` (Direkomendasikan)
1. Salin berkas `plugin.zip` (atau `luai-acode-1.0.0.zip`) ke memori penyimpanan HP Android Anda.
2. Buka aplikasi **Acode**.
3. Buka menu **Settings** -> **Plugins**.
4. Klik tombol **+** (atau ikon menu titik tiga) lalu pilih **Install from zip file**.
5. Cari dan pilih berkas `plugin.zip` Luai.
6. Muat ulang (*restart*) aplikasi Acode.

### Cara 2: Instalasi Manual (Salin Folder)
Salin folder `luai-acode` ke direktori penyimpanan plugin internal Acode di Android:
```text
/sdcard/Android/data/com.foxdebug.acode/files/plugins/com.luailang.acode
```

---

## Cara Menjalankan Kode Luai di Acode

1. Buat atau buka berkas dengan ekstensi `.luai` (contoh: `program.luai`).
2. Tulis kode bahasa Luai:
   ```lua
   lokal nama = "Pengguna Android"
   cetak("Halo, " .. nama .. "!")
   
   untuk i = 1, 5 lakukan
       cetak("Nilai ke-", i, "kuadrat:", i * i)
   selesai
   
   cetak("Akar 225 =", matematika.akar(225))
   ```
3. Tekan tombol mengambang hijau **Play ▶️** di kanan bawah layar.
4. Jendela Terminal Console Luai akan langsung terbuka menampilkan hasil output program Anda.
