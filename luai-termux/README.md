# Luai Native Binary untuk Termux (Android)

Paket binary ELF native mandiri (*statically linked*) bahasa pemrograman **Luai** (dialek Bahasa Indonesia dari Lua 5.4) yang dirancang khusus untuk lingkungan terminal **Termux** di Android.

---

## Fitur Utama

- **100% Native & Terisolasi Mandiri (*Fully Static Binary*)**:
  - Dibuat menggunakan kompilasi statis sehingga binary tidak bergantung pada shared library eksternal. Dapat langsung dijalankan di semua versi Termux dan Android (ARM64 & x86_64).
- **Mode Eksekusi Berkas Langsung**:
  - Menjalankan berkas skrip `.luai` secara langsung melalui baris perintah Termux: `luai skrip.luai`.
- **Mode Interaktif REPL Penuh**:
  - Jalankan `luai` tanpa argumen untuk masuk ke interactive shell dengan prompt `luai> `.
  - Mendukung evaluasi ekspresi otomatis, blok baris jamak (`>> `), dan terminasi melalui perintah `keluar` atau `exit`.
- **Dukungan Evaluasi Satu Baris (`-e`)**:
  - `luai -e "cetak('Halo dari Termux!')"`

---

## Berkas yang Disediakan

| Berkas | Arsitektur | Keterangan |
|:---|:---|:---|
| `luai` | ARM64 (`aarch64`) | Binary utama untuk seluruh HP Android 64-bit modern |
| `luai-arm64` | ARM64 (`aarch64`) | Binary versi ARM64 spesifik |
| `luai-x86_64` | x86_64 | Binary untuk Emulator Android / PC x86_64 |
| `install.sh` | Bash Script | Skrip instalasi otomatis ke `$PREFIX/bin/luai` |
| `test_luai.luai` | Luai Script | Skrip pengujian alur logika & sintaks |
| `modul_kalkulator.luai` | Luai Module | Contoh pustaka modul Luai |

---

## Panduan Pemasangan di Termux

### Metode 1: Instalasi Otomatis (Direkomendasikan)
1. Pindahkan folder `luai-termux` ke Termux (misalnya via `termux-setup-storage` dari memori internal atau `scp`/`git`).
2. Buka terminal Termux dan masuk ke folder `luai-termux`:
   ```bash
   cd luai-termux
   ```
3. Berikan izin eksekusi dan jalankan skrip instalasi:
   ```bash
   chmod +x install.sh
   bash install.sh
   ```
4. Selesai! Sekarang perintah `luai` sudah terpasang secara global di Termux Anda.

---

### Metode 2: Instalasi Manual ke PATH Termux
Salin binary `luai` ke direktori binary Termux:
```bash
cp luai $PREFIX/bin/luai
chmod +x $PREFIX/bin/luai
```

---

### Metode 3: Eksekusi Langsung Tanpa Instalasi
Anda juga dapat langsung mengeksekusi binary lokal di dalam folder:
```bash
chmod +x luai
./luai test_luai.luai
```

---

## Contoh Penggunaan di Termux

### 1. Menjalankan Berkas Kode Luai
```bash
luai test_luai.luai
```
**Output:**
```text
===========================================
Pengujian Bahasa Pemrograman Luai v1.0.0
===========================================
Predikat: Memuaskan (B)
Faktorial dari 5 adalah:	120
Perulangan 'untuk' (1 sampai 5):
  Iterasi ke-	1	kuadrat:	1
  Iterasi ke-	2	kuadrat:	4
  Iterasi ke-	3	kuadrat:	9
  Iterasi ke-	4	kuadrat:	16
  Iterasi ke-	5	kuadrat:	25
Perulangan 'selama' (hitungan mundur):
  Hitungan mundur:	3
  Hitungan mundur:	2
  Hitungan mundur:	1
Pengujian selesai dengan sukses!
```

### 2. Mode Interaktif (REPL) di Termux
```bash
luai
```
```text
Luai 1.0.0 (Lua dialek Bahasa Indonesia) [Lua 5.4.7]
Ketik "keluar" atau "exit" untuk mengakhiri sesi interaktif.

luai> 15 * 8
120
luai> matematika.akar(625)
25.0
luai> lokal pesan = "Selamat datang di Luai Termux!"
luai> cetak(teks.huruf_besar(pesan))
SELAMAT DATANG DI LUAI TERMUX!
luai> keluar
```

### 3. Eksekusi Perintah Satu Baris
```bash
luai -e "untuk i = 1, 3 lakukan cetak('Iterasi:', i) selesai"
```
