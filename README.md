<p align="center">
  <img src="assets/logo.png" alt="Logo Luai" width="150"/>
</p>

# Luai - Bahasa Pemrograman Dialek Bahasa Indonesia (dengan JIT Compiler)

**Luai** adalah implementasi bahasa pemrograman dinamis modern berbasis dialek Bahasa Indonesia yang ditenagai oleh **LuaJIT (Just-In-Time Compiler)** berkecepatan tinggi. Seluruh kata kunci resmi, operator logika kata, fungsi bawaan, dan modul standar telah dilokalisasi ke dalam Bahasa Indonesia dengan pemisah garis bawah (`_`) untuk kata majemuk.

Dengan integrasi JIT Compiler, eksekusi kode Luai berjalan sangat cepat setara dengan bahasa terkompilasi native, sembari tetap mempertahankan kemudahan sintaksis Bahasa Indonesia yang ramah bagi pemula maupun profesional.

---

## Daftar Isi
1. [Fitur Utama](#fitur-utama)
2. [Langkah-Langkah Instalasi](#langkah-langkah-instalasi)
3. [Panduan Penggunaan Interpreter (Cara Pakai)](#panduan-penggunaan-interpreter-cara-pakai)
4. [Tutorial Kilat Luai (Crash Course 5 Menit)](#tutorial-kilat-luai-crash-course-5-menit)
5. [Tabel Kamus Sintaks Resmi (22 Kata Kunci)](#tabel-kamus-sintaks-resmi-22-kata-kunci)
6. [Library Standar & Input Pengguna (`dasar` & `io`)](#library-standar--input-pengguna-dasar--io)
7. [Modul Terjemahan Lainnya](#modul-terjemahan-lainnya)
8. [Ekstensi Editor & IDE](#ekstensi-editor--ide)
9. [Struktur Direktori Proyek](#struktur-direktori-proyek)

---

## Fitur Utama

- **Ditenagai LuaJIT Compiler (Sangat Cepat)**: Memanfaatkan mesin kompilasi *Trace-based JIT* dari LuaJIT 2.1 untuk performa eksekusi tingkat tinggi.
- **Sintaks Bahasa Indonesia Baku**: 22 kata kunci Lua dipetakan ke Bahasa Indonesia yang konsisten dan ekspresif.
- **Library `dasar` & `io` Interaktif**: Mendukung penuh fungsi interaktif untuk meminta masukan pengguna (`masukan`, `minta`, `tanya`, `baca`) dengan prompt teks langsung.
- **Dukungan Multi-Modul**: Pengimporan berkas modul `.luai` otomatis melalui fungsi `butuh("nama_modul")` atau `perlu("nama_modul")`.
- **REPL Interaktif Cerdas**: Interactive shell interaktif dengan prompt `luai> `, evaluasi ekspresi otomatis, blok multiline `>> `, dan penanganan keluar intuitif (`keluar` / `exit`).
- **Binary Mandiri (*Zero Dependency*)**: Executable statis mandiri, tidak memerlukan instalasi runtime eksternal.

---

## Langkah-Langkah Instalasi

### 1. Menggunakan Binary yang Sudah Jadi (Windows)
Jika Anda telah mengunduh atau mengkloning repositori ini, berkas executable `luai.exe` sudah tersedia di folder utama.

#### Menambahkan `luai` ke System PATH (Agar bisa dipanggil dari terminal mana saja):
1. Tekan tombol `Win + S`, ketik **Environment Variables** (atau *Edit the system environment variables*), lalu tekan **Enter**.
2. Klik tombol **Environment Variables...** di bagian bawah.
3. Pada bagian **User variables** (atau **System variables**), pilih baris **Path** lalu klik **Edit...**.
4. Klik **New**, lalu masukkan path direktori tempat `luai.exe` berada (misalnya: `C:\Users\NamaAnda\Downloads\Luai`).
5. Klik **OK** pada semua jendela.
6. Buka jendela terminal baru (PowerShell / Command Prompt), lalu ketik:
   ```powershell
   luai -v
   ```
   Jika muncul informasi versi Luai dengan status JIT aktif, instalasi berhasil!

---

### 2. Kompilasi Ulang dari Source Code (Build Ulang)

#### Persyaratan:
- Kompiler C++17 (misalnya `g++` MinGW, `clang++`, atau LLVM-MinGW).
- `mingw32-make` atau `make`.

#### Menjalankan Build Otomatis:
Cukup jalankan berkas batch yang telah disediakan di terminal Windows:
```bat
.\build.bat
```
Skrip ini akan:
1. Mengompilasi pustaka statis `libluajit.a` dari folder `luajit/` jika belum tersedia.
2. Mengompilasi seluruh modul C++ Luai (`src/lexer.cpp`, `src/runtime.cpp`, `src/repl.cpp`, `src/main.cpp`) ke berkas executable `luai.exe`.
3. Memperbarui binary di dalam ekstensi VS Code secara otomatis.

---

### 3. Instalasi di Android (Termux) — Tanpa Perlu Build

Anda **TIDAK PERLU** mengompilasi atau build apa pun di Termux karena binary native siap pakai (**ARM 64-bit**, **ARM 32-bit**, dan **x86_64**) sudah disediakan secara lengkap. Kompilasi otomatis Termux juga di-build langsung melalui GitHub Actions CI/CD.

#### Metode A: Instalasi Kilat 1 Baris Perintah (Direkomendasikan)
Buka aplikasi **Termux** di perangkat Android Anda, pastikan memiliki koneksi internet, lalu jalankan perintah berikut:
```bash
curl -sL https://raw.githubusercontent.com/sunandar3221/Luai/main/luai-termux/install.sh | bash
```
> **Catatan:** Skrip pintar ini otomatis mendeteksi arsitektur CPU perangkat Anda secara akurat (apakah **ARM 64-bit** `aarch64`, **ARM 32-bit** `armv7l` / `armhf`, atau **x86_64`), mengunduh binary yang sesuai dari repositori/rilis GitHub, memasangnya langsung ke `$PREFIX/bin/luai`, dan memverifikasi kesehatannya.

#### Metode B: Mengunduh Arsip dari GitHub Release
1. Unduh berkas `luai-termux.tar.gz` langsung dari halaman [GitHub Releases](https://github.com/sunandar3221/Luai/releases):
   ```bash
   curl -LO https://github.com/sunandar3221/Luai/releases/download/v1.0.0/luai-termux.tar.gz
   tar -xzf luai-termux.tar.gz
   cd luai-termux
   chmod +x install.sh
   ./install.sh
   ```

#### Metode C: Salin Manual Binary Langsung
Jika Anda menyukai cara manual, Anda bisa langsung mengunduh binary ke folder bin Termux:
```bash
# Untuk HP Android (ARM64 / AArch64):
curl -sL https://raw.githubusercontent.com/sunandar3221/Luai/main/luai-termux/luai-arm64 -o $PREFIX/bin/luai
chmod +x $PREFIX/bin/luai

# Verifikasi instalasi:
luai -v
```
Ketik `luai` di Termux kapan saja untuk membuka shell interaktif REPL atau `luai skrip.luai` untuk menjalankan program.

---

## Panduan Penggunaan Interpreter (Cara Pakai)

### 1. Mode Interaktif (REPL)
Jalankan `luai` tanpa argumen untuk membuka shell interaktif:
```powershell
.\luai.exe
```
Tampilan REPL:
```text
Luai 1.0.0 (Dialek Bahasa Indonesia) [LuaJIT JIT Aktif]
Ketik "keluar" atau "exit" untuk mengakhiri sesi interaktif.

luai> 10 + 25
35
luai> matematika.akar(144)
12
luai> lokal nama = dasar.masukan("Nama Anda: ")
Nama Anda: Budi
luai> cetak("Halo, " .. nama)
Halo, Budi
luai> keluar
```

### 2. Mode Eksekusi Berkas Skrip (`.luai`)
Jalankan file program Luai dengan menyertakan nama berkas:
```powershell
.\luai.exe skrip.luai
```
Anda juga bisa menyertakan argumen tambahan:
```powershell
.\luai.exe skrip.luai argumen1 argumen2
```
Argumen akan tersimpan di tabel global `arg`.

### 3. Mode Satu Baris Kode (`-e`)
Jalankan kode Luai langsung dari baris perintah:
```powershell
.\luai.exe -e "untuk i = 1, 3 lakukan cetak('Hitungan ke-', i) selesai"
```

### 4. Mode Interaktif Setelah Eksekusi Skrip (`-i`)
Jalankan skrip terlebih dahulu, lalu masuk ke REPL dengan status lingkungan skrip tetap tersimpan:
```powershell
.\luai.exe -i skrip.luai
```

### 5. Memeriksa Versi & Bantuan
```powershell
.\luai.exe -v
.\luai.exe -h
```

---

## Tutorial Kilat Luai (Crash Course 5 Menit)

Pelajari dasar-dasar bahasa Luai dalam 5 menit melalui panduan praktis berikut:

### 1. Variabel & Tipe Data
Gunakan kata kunci `lokal` untuk mendefinisikan variabel lokal:
```lua
lokal nama = "Budi Santoso"     -- teks (string)
lokal usia = 21                 -- angka (number)
lokal nilai = 87.5              -- angka desimal
lokal aktif = benar             -- boolean (benar / salah)
lokal data_kosong = nihil       -- nihil / kosong (nil)

cetak("Nama :", nama)
cetak("Usia :", usia)
cetak("Aktif:", aktif)          -- otomatis dicetak sebagai 'benar'
```

### 2. Meminta Input dari Pengguna (`dasar` & `io`)
Luai menyediakan fungsi interaktif bawaan untuk meminta input pengguna dengan pesan teks (prompt):
```lua
-- Menggunakan library 'dasar'
lokal nama = dasar.masukan("Masukkan nama Anda: ")

-- Menggunakan library 'io'
lokal umur = io.masukan("Masukkan umur Anda: ")

-- Atau menggunakan alias fungsi:
lokal hobi = io.minta("Hobi Anda: ")
lokal kota = io.tanya("Kota domisili: ")

cetak("Halo " .. nama .. ", umur Anda " .. umur .. " tahun dari " .. kota .. "!")
```

### 3. Percabangan Kondisi (`jika`, `maka`, `atau_jika`, `selain_itu`, `selesai`)
```lua
lokal nilai = 85

jika nilai >= 90 maka
    cetak("Predikat: A (Istimewa)")
atau_jika nilai >= 80 maka
    cetak("Predikat: B (Bagus)")
selain_itu
    cetak("Predikat: C (Cukup)")
selesai
```

### 4. Perulangan (`untuk`, `selama`, `ulang ... sampai`)
#### Perulangan Angka (`untuk`):
```lua
untuk i = 1, 5 lakukan
    cetak("Iterasi ke-", i)
selesai
```

#### Perulangan Berbasis Kondisi (`selama`):
```lua
lokal sisa = 3
selama sisa > 0 lakukan
    cetak("Menghitung mundur:", sisa)
    sisa = sisa - 1
selesai
```

#### Perulangan dengan Kondisi di Akhir (`ulang ... sampai`):
```lua
lokal x = 1
ulang
    jika x == 3 maka
        cetak("Melewati angka 3 dan berhenti")
        berhenti
    selesai
    cetak("Nilai x:", x)
    x = x + 1
sampai x > 5
```

### 5. Fungsi & Nilai Kembalian (`fungsi`, `kembalikan`)
```lua
fungsi hitung_luas(panjang, lebar)
    kembalikan panjang * lebar
selesai

lokal luas = hitung_luas(10, 5)
cetak("Luas persegi panjang:", luas)
```

Fungsi tingkat tinggi (*first-class closures*):
```lua
fungsi pengali(faktor)
    kembalikan fungsi(nilai)
        kembalikan nilai * faktor
    selesai
selesai

lokal kali_dua = pengali(2)
cetak("5 dikali 2 =", kali_dua(5))  -- 10
```

### 6. Tabel, Array, dan Iterator
Tabel adalah struktur data utama di Luai:
```lua
-- Array bernomor indeks (dimulai dari indeks 1)
lokal daftar_buah = {"Apel", "Mangga", "Jeruk"}
tabel.sisip(daftar_buah, "Pisang")

-- Iterasi array dengan i_pasangan
untuk indeks, buah dalam i_pasangan(daftar_buah) lakukan
    cetak("Buah ke-" .. indeks .. ":", buah)
selesai

-- Membongkar elemen tabel (unpack)
lokal a, b, c = lepas(daftar_buah)
cetak("Tiga buah pertama:", a, b, c)

-- Tabel asosiatif (kamus / dictionary)
lokal mahasiswa = {
    nama = "Siti",
    jurusan = "Teknik Informatika",
    ipk = 3.9
}

untuk kunci, nilai dalam pasangan(mahasiswa) lakukan
    cetak(kunci, ":", nilai)
selesai
```

### 7. Pemrograman Berorientasi Objek (OOP)
```lua
lokal Kucing = {}
Kucing.__index = Kucing

fungsi Kucing.baru(nama)
    lokal objek = set_metatabel({}, Kucing)
    objek.nama = nama
    kembalikan objek
selesai

fungsi Kucing:bersuara()
    cetak(self.nama .. " bersuara: Meong!")
selesai

lokal mpus = Kucing.baru("Kitty")
mpus:bersuara()
```

### 8. Memuat Modul Terpisah (`butuh` / `perlu`)
Buat berkas `matematika_kustom.luai`:
```lua
lokal M = {}
fungsi M.kuadrat(x)
    kembalikan x * x
selesai
kembalikan M
```
Lalu panggil di skrip utama:
```lua
lokal modul = butuh("matematika_kustom")
cetak("Kuadrat dari 7:", modul.kuadrat(7))
```

### 9. Memeriksa Status JIT Compiler
```lua
lokal aktif, mode = jit.status()
cetak("JIT Engine Aktif:", aktif)       -- benar
cetak("Versi LuaJIT    :", jit.version) -- LuaJIT 2.1...
```

---

## Tabel Kamus Sintaks Resmi (22 Kata Kunci)

| No | Kata Kunci Lua | Kata Kunci Luai | Keterangan & Contoh Penggunaan |
|:---|:---|:---|:---|
| 1 | `and` | `dan` | Operator logika konjungsi (`jika a dan b maka`) |
| 2 | `break` | `berhenti` | Menghentikan paksa iterasi perulangan |
| 3 | `do` | `lakukan` | Pembuka blok eksekusi (`untuk i = 1, 10 lakukan`) |
| 4 | `else` | `selain_itu` | Percabangan kondisi alternatif terakhir |
| 5 | `elseif` | `atau_jika` | Percabangan kondisi alternatif bertingkat |
| 6 | `end` | `selesai` | Penutup blok struktur kontrol atau fungsi |
| 7 | `false` | `salah` | Nilai boolean salah / palsu |
| 8 | `for` | `untuk` | Perulangan numerik atau iterator generik |
| 9 | `function` | `fungsi` | Deklarasi fungsi baru atau fungsi anonim (*lambda*) |
| 10 | `goto` | `lompat_ke` | Lompat ke label target (`lompat_ke label`) |
| 11 | `if` | `jika` | Percabangan kondisi utama |
| 12 | `in` | `dalam` | Iterator penjelajah elemen (`dalam pasangan(t)`) |
| 13 | `local` | `lokal` | Deklarasi variabel atau fungsi berlingkup lokal |
| 14 | `nil` | `kosong` / `nihil` | Nilai kosong / non-eksistensi (keduanya didukung) |
| 15 | `not` | `bukan` | Operator logika negasi (`jika bukan status maka`) |
| 16 | `or` | `atau` | Operator logika disjungsi (`jika x atau y maka`) |
| 17 | `repeat` | `ulang` | Perulangan dengan evaluasi kondisi di akhir |
| 18 | `return` | `kembalikan` | Mengembalikan nilai dari dalam fungsi |
| 19 | `then` | `maka` | Pembuka blok konsekuensi percabangan `jika` |
| 20 | `true` | `benar` | Nilai boolean benar |
| 21 | `until` | `sampai` | Kondisi terminasi untuk perulangan `ulang` |
| 22 | `while` | `selama` | Perulangan berbasis kondisi di awal |

---

## Library Standar & Input Pengguna (`dasar` & `io`)

### 1. Meminta Input dari Pengguna
Luai menyediakan dukungan meminta masukan langsung dari pengguna:

| Pemanggilan | Contoh | Penjelasan |
|:---|:---|:---|
| `dasar.masukan(pesan)` | `dasar.masukan("Nama: ")` | Menampilkan prompt dan membaca baris masukan teks pengguna |
| `dasar.minta(pesan)` | `dasar.minta("Umur: ")` | Alias dari `dasar.masukan` |
| `dasar.tanya(pesan)` | `dasar.tanya("Kota: ")` | Alias dari `dasar.masukan` |
| `dasar.baca([format_atau_pesan])` | `dasar.baca("Alamat: ")` | Membaca masukan dengan pesan atau format tertentu |
| `io.masukan(pesan)` | `io.masukan("Email: ")` | Menampilkan prompt dan membaca masukan melalui modul `io` |
| `io.minta(pesan)` | `io.minta("Password: ")` | Alias input pada modul `io` |
| `io.tanya(pesan)` | `io.tanya("Y/N: ")` | Alias input pada modul `io` |
| `io.baca([format_atau_pesan])` | `io.baca("Ketik: ")` | Membaca input (mendukung prompt atau format standar seperti `"*n"`) |
| `masukan(pesan)` | `masukan("Nama: ")` | Fungsi global mandiri untuk meminta input pengguna |
| `minta(pesan)` | `minta("Umur: ")` | Alias fungsi global |
| `tanya(pesan)` | `tanya("Konfirmasi: ")` | Alias fungsi global |

Contoh Penggunaan Input Lengkap:
```lua
lokal nama = dasar.masukan("Nama Anda   : ")
lokal umur = io.masukan("Umur Anda   : ")
lokal hobi = io.minta("Hobi Anda   : ")

cetak("=========================")
cetak("Data Pengguna:")
cetak("Nama:", nama)
cetak("Umur:", umur)
cetak("Hobi:", hobi)
```

### 2. Isi Lengkap Library `dasar`
Library `dasar` (dan alias kompatibilitas `daar`) menyediakan seluruh fungsi inti bahasa:
- `dasar.masukan(prompt)` / `dasar.minta(prompt)` / `dasar.tanya(prompt)` / `dasar.baca(prompt)`
- `dasar.cetak(...)`: Menampilkan nilai ke terminal (boolean & nihil otomatis diformat Bahasa Indonesia).
- `dasar.tipe(nilai)`: Mengembalikan nama tipe (`"angka"`, `"teks"`, `"tabel"`, `"fungsi"`, `"boolean"`, `"nihil"`, `"korutin"`).
- `dasar.ke_angka(nilai, [basis])`: Konversi teks ke angka.
- `dasar.ke_teks(nilai)`: Konversi nilai ke teks.
- `dasar.pasangan(tabel)`: Iterator pasangan kunci-nilai.
- `dasar.i_pasangan(tabel)`: Iterator array indeks bernomor.
- `dasar.lepas(tabel, [i], [j])`: Membongkar elemen tabel menjadi daftar nilai.
- `dasar.tegaskan(kondisi, [pesan])`: Penegasan kebenaran logika (*assert*).
- `dasar.kesalahan(pesan)`: Melempar pesan kesalahan (*error*).
- `dasar.panggil_aman(fungsi, ...)`: Menjalankan fungsi dalam proteksi *pcall*.
- `dasar.set_metatabel(tabel, meta)`: Menetapkan metatable.
- `dasar.ambil_metatabel(tabel)`: Mengambil metatable.
- `dasar.koleksi_sampah([opsi])`: Manajemen memori *garbage collector*.
- `dasar.butuh(nama_modul)` / `dasar.perlu(nama_modul)`: Mengimpor modul berkas `.luai`.

---

## Modul Terjemahan Lainnya

### 1. Modul `matematika` (`math`)
- `matematika.akar(x)`: Akar kuadrat (`sqrt`)
- `matematika.mutlak(x)`: Nilai absolut (`abs`)
- `matematika.lantai(x)`: Pembulatan ke bawah (`floor`)
- `matematika.atap(x)`: Pembulatan ke atas (`ceil`)
- `matematika.acak([m], [n])`: Bilangan acak (`random`)
- `matematika.benih_acak(x)`: Inisialisasi seed acak (`randomseed`)
- `matematika.minimum(...)`: Nilai terkecil (`min`)
- `matematika.maksimum(...)`: Nilai terbesar (`max`)
- `matematika.derajat(rad)`: Radian ke derajat (`deg`)
- `matematika.radian(deg)`: Derajat ke radian (`rad`)
- `matematika.pi`, `matematika.sin`, `matematika.cos`, `matematika.tan`, dll.

### 2. Modul `tabel` (`table`)
- `tabel.sisip(t, [pos], val)`: Menyisipkan elemen (`insert`)
- `tabel.hapus(t, [pos])`: Menghapus elemen (`remove`)
- `tabel.urut(t, [fungsi])`: Mengurutkan elemen tabel (`sort`)
- `tabel.gabung(t, [pemisah])`: Menggabungkan elemen tabel menjadi string (`concat`)
- `tabel.pindah(a1, f, e, t)`: Memindahkan rentang elemen (`move`)
- `tabel.bungkus(...)`: Membungkus daftar argumen ke tabel (`pack`)
- `tabel.lepas(t, [i], [j])`: Membongkar tabel menjadi argumen (`unpack`)

### 3. Modul `teks` (`string`)
- `teks.panjang(s)`: Panjang karakter string (`len`)
- `teks.huruf_besar(s)`: Menjadi huruf kapital (`upper`)
- `teks.huruf_kecil(s)`: Menjadi huruf kecil (`lower`)
- `teks.potong(s, i, [j])`: Memotong substring (`sub`)
- `teks.cari(s, pola)`: Menemukan posisi pola (`find`)
- `teks.ganti(s, pola, ganti)`: Mengganti teks pola regex/string (`gsub`)
- `teks.format(fmt, ...)`: Memformat string (`format`)
- `teks.ulang(s, n)`: Mengulang string n kali (`rep`)
- `teks.balik(s)`: Membalik urutan karakter (`reverse`)

### 4. Modul `sistem` (`os`)
- `sistem.waktu()`: Mengambil timestamp UNIX (`time`)
- `sistem.jam()`: Waktu CPU yang digunakan (`clock`)
- `sistem.tanggal([format])`: Format tanggal dan waktu (`date`)
- `sistem.keluar([kode])`: Menghentikan eksekusi program (`exit`)
- `sistem.jalankan(perintah)`: Menjalankan perintah OS (`execute`)
- `sistem.ambil_env(variabel)`: Mengambil environment variable (`getenv`)
- `sistem.hapus(nama_file)`: Menghapus file dari disk (`remove`)
- `sistem.ganti_nama(lama, baru)`: Mengubah nama file (`rename`)

### 5. Modul `io`
- `io.masukan(pesan)`: Meminta dan membaca input baris dari pengguna.
- `io.minta(pesan)` / `io.tanya(pesan)`: Alias untuk meminta masukan pengguna.
- `io.baca([format_atau_pesan])`: Membaca masukan pengguna atau stream file.
- `io.tulis(...)`: Menulis ke terminal / keluaran standar (`write`).
- `io.buka(nama_file, [mode])`: Membuka file (`open`).
- `io.tutup([file])`: Menutup file handler (`close`).
- `io.siram()`: Membilas buffer keluaran (`flush`).
- `io.baris([nama_file])`: Iterator membaca baris per baris (`lines`).
- `io.berkas_masukan([file])`: Mengatur file masukan *default* (`input`).
- `io.berkas_keluaran([file])`: Mengatur file keluaran *default* (`output`).

### 6. Modul `korutin` (`coroutine`)
- `korutin.buat(fungsi)`: Membuat instance korutin baru (`create`)
- `korutin.lanjut(ko, ...)`: Menjalankan / melanjutkan korutin (`resume`)
- `korutin.hasil(...)`: Menyerahkan eksekusi dari dalam korutin (`yield`)
- `korutin.status(ko)`: Memeriksa status: `"suspended"`, `"running"`, `"dead"` (`status`)
- `korutin.bungkus(fungsi)`: Membuat fungsi wrapper korutin (`wrap`)

---

## Ekstensi Editor & IDE

### 1. Visual Studio Code ([`luai-vscode/`](luai-vscode/))
- **Fitur**: Pewarnaan sintaks resmi (*syntax highlighting*), Tombol Jalankan Berkas (Play ▶️), REPL Terminal interaktif, snippets otomatis, serta binary compiler bawaan.
- **Installer**: Pasang langsung berkas [`luai-vscode/luai-vscode-1.1.0.vsix`](luai-vscode/luai-vscode-1.1.0.vsix) melalui menu *Install from VSIX...* di VS Code.

### 2. Acode Editor untuk Android ([`luai-acode/`](luai-acode/))
- **Fitur**: Syntax highlighting Ace Mode, console runner mandiri tanpa termux/root, tombol Play (▶️), dan snippets.
- **Installer**: Pasang plugin zip [`luai-acode/plugin.zip`](luai-acode/plugin.zip) pada menu *Plugins* aplikasi Acode.

### 3. Termux Android Binary Native ([`luai-termux/`](luai-termux/))
- **Fitur**: Binary ELF static mandiri untuk ARM64 & x86_64 Termux di Android lengkap dengan skrip instalasi cepat `install.sh`.

---

## Struktur Direktori Proyek

```text
Luai/
├── src/
│   ├── lexer.hpp           # Header lexer / transpiler sintaks Bahasa Indonesia
│   ├── lexer.cpp           # Tokenisasi, kata kunci, operator //, konkat angka, & member access
│   ├── runtime.hpp         # Header runtime engine Luai & binding LuaJIT C API
│   ├── runtime.cpp         # Implementasi library dasar, io, modul terjemahan, & JIT bridging
│   ├── repl.hpp            # Header interaktif REPL shell
│   ├── repl.cpp            # Implementasi interaktif REPL & multi-line support
│   └── main.cpp            # Titik masuk utama program CLI & versi
├── luajit/                 # Mesin inti LuaJIT 2.1 Just-In-Time Compiler
│   ├── src/
│   │   ├── libluajit.a     # Library statis LuaJIT terkompilasi
│   │   ├── lua.hpp
│   │   ├── luajit.h
│   │   └── ...
│   └── Makefile
├── examples/
│   ├── demo_lengkap.luai             # Demonstrasi lengkap seluruh fitur
│   ├── perulangan_dan_fungsi.luai    # Demonstrasi kontrol alur, fungsi, & pembagian bulat //
│   ├── struktur_data.luai            # Demonstrasi tabel & OOP metatables
│   └── korutin_dan_modul.luai        # Demonstrasi korutin & import modul
├── luai-vscode/            # Ekstensi resmi Visual Studio Code (VSIX + Bundled Runner)
│   ├── bin/luai.exe
│   ├── extension.js
│   ├── package.json
│   └── README.md
├── luai-acode/             # Ekstensi resmi Acode Android (Plugin Zip + In-App JS Runner)
├── luai-termux/            # Paket Binary Native Termux Android (ARM64 & x86_64)
├── test_luai.luai          # Skrip pengujian utama
├── test_fitur_baru.luai    # Skrip pengujian perbaikan bug & fitur baru
├── test_input.luai         # Contoh skrip interaktif input pengguna (dasar & io)
├── modul_kalkulator.luai   # Contoh modul terpisah untuk pengujian import
├── build.bat               # Skrip build otomatis menggunakan LuaJIT
├── luai.exe                # Executable binary interpreter Windows dengan JIT
└── README.md               # Dokumentasi resmi bahasa pemrograman Luai
```
