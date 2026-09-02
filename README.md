# Luai - Bahasa Pemrograman Dialek Bahasa Indonesia

**Luai** adalah implementasi bahasa pemrograman dinamis modern berbasis dialek Bahasa Indonesia yang kompatibel 100% secara fungsional dengan fitur inti **Lua 5.4**. Seluruh kata kunci resmi, operator logika kata, fungsi bawaan, dan modul standar telah dilokalisasi ke dalam Bahasa Indonesia dengan pemisah garis bawah (`_`) untuk kata majemuk.

---

## Daftar Isi
1. [Fitur Utama](#fitur-utama)
2. [Tabel Kamus Sintaks Resmi (22 Kata Kunci)](#tabel-kamus-sintaks-resmi-22-kata-kunci)
3. [Fungsi Global & Modul Bawaan](#fungsi-global--modul-bawaan)
4. [Panduan Build & Kompilasi](#panduan-build--kompilasi)
5. [Panduan Penggunaan Interpreter](#panduan-penggunaan-interpreter)
6. [Contoh Kode Program](#contoh-kode-program)
7. [Struktur Direktori Proyek](#struktur-direktori-proyek)

---

## Fitur Utama

- **100% Kompatibel dengan Lua Core**: Mendukung penuh *First-Class Functions*, *Closures*, *Tables/Associative Arrays*, *Metatables* (OOP), *Coroutines*, *Dynamic Typing*, dan *Garbage Collection*.
- **Sintaks Bahasa Indonesia Baku**: 22 kata kunci Lua dipetakan ke Bahasa Indonesia yang konsisten dan ekspresif.
- **Dukungan Multi-Modul**: Pengimporan berkas modul `.luai` otomatis melalui fungsi `butuh("nama_modul")` atau `perlu("nama_modul")`.
- **REPL Interaktif Cerdas**: Interactive shell interaktif dengan prompt `luai> `, evaluasi ekspresi otomatis, blok multiline `>> `, dan penanganan keluar yang intuitif (`keluar` / `exit`).
- **Engine Modern C++**: Arsitektur modular yang cepat, efisien, mandiri (*zero external runtime dependencies*), dan siap digunakan.

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
| 14 | `nil` | `kosong` / `nihil` | Nilai kosong / non-eksistensi (kedua kata kunci didukung) |
| 15 | `not` | `bukan` | Operator logika negasi (`jika bukan status maka`) |
| 16 | `or` | `atau` | Operator logika disjungsi (`jika x atau y maka`) |
| 17 | `repeat` | `ulang` | Perulangan dengan evaluasi kondisi di akhir |
| 18 | `return` | `kembalikan` | Mengembalikan nilai dari dalam fungsi |
| 19 | `then` | `maka` | Pembuka blok konsekuensi percabangan `jika` |
| 20 | `true` | `benar` | Nilai boolean benar |
| 21 | `until` | `sampai` | Kondisi terminasi untuk perulangan `ulang` |
| 22 | `while` | `selama` | Perulangan berbasis kondisi di awal |

---

## Fungsi Global & Modul Bawaan

### 1. Fungsi Global

| Fungsi Luai | Padanan Lua | Deskripsi |
|:---|:---|:---|
| `cetak(...)` | `print(...)` | Menampilkan nilai ke terminal (boolean & nihil otomatis diformat Bahasa Indonesia) |
| `tipe(nilai)` | `type(nilai)` | Menghasilkan nama tipe data: `"angka"`, `"teks"`, `"tabel"`, `"fungsi"`, `"boolean"`, `"nihil"`, `"korutin"` |
| `ke_angka(nilai, [basis])` | `tonumber(...)` | Mengonversi teks ke angka |
| `ke_teks(nilai)` | `tostring(...)` | Mengonversi nilai ke representasi teks string |
| `pasangan(tabel)` | `pairs(...)` | Iterator untuk seluruh pasangan kunci-nilai tabel |
| `i_pasangan(tabel)` | `ipairs(...)` | Iterator berurutan untuk tabel array bernomor indeks |
| `masukan([format_atau_teks])` | `io.read(...)` | Membaca baris masukan pengguna dari keyboard / terminal |
| `lepas(tabel, [i], [j])` | `table.unpack(...)`| Membongkar elemen tabel array menjadi daftar argumen |
| `butuh(nama_modul)` | `require(...)` | Memuat modul eksternal berekstensi `.luai` |
| `perlu(nama_modul)` | `require(...)` | Alias untuk `butuh` |
| `tegaskan(kondisi, [pesan])` | `assert(...)` | Menegaskan kebenaran kondisi logika |
| `kesalahan(pesan)` | `error(...)` | Melemparkan galat (exception) dengan pesan tertentu |
| `panggil_aman(fungsi, ...)` | `pcall(...)` | Menjalankan fungsi dalam mode terproteksi |
| `set_metatabel(tabel, meta)` | `setmetatable(...)`| Menetapkan metatable untuk tabel target |
| `ambil_metatabel(tabel)` | `getmetatable(...)`| Mengambil metatable dari sebuah objek tabel |
| `koleksi_sampah([opsi])` | `collectgarbage(...)`| Mengatur siklus pengumpulan sampah memori |

### 2. Modul Standar Terjemahan

#### Modul `matematika` (`math`)
- `matematika.akar(x)`: Akar kuadrat (`sqrt`)
- `matematika.mutlak(x)`: Nilai absolut (`abs`)
- `matematika.lantai(x)`: Pembulatan ke bawah (`floor`)
- `matematika.atap(x)`: Pembulatan ke atas (`ceil`)
- `matematika.acak([m], [n])`: Bilangan acak (`random`)
- `matematika.benih_acak(x)`: Inisialisasi seed bilangan acak (`randomseed`)
- `matematika.minimum(...)`: Nilai terkecil (`min`)
- `matematika.maksimum(...)`: Nilai terbesar (`max`)
- `matematika.derajat(rad)`: Konversi radian ke derajat (`deg`)
- `matematika.radian(deg)`: Konversi derajat ke radian (`rad`)
- `matematika.pi`, `matematika.sin`, `matematika.cos`, `matematika.tan`, `matematika.log`, dll.

#### Modul `tabel` (`table`)
- `tabel.sisip(t, [pos], val)`: Menyisipkan elemen baru (`insert`)
- `tabel.hapus(t, [pos])`: Menghapus elemen (`remove`)
- `tabel.urut(t, [fungsi])`: Mengurutkan elemen tabel (`sort`)
- `tabel.gabung(t, [pemisah])`: Menggabungkan elemen tabel menjadi satu string (`concat`)
- `tabel.pindah(a1, f, e, t)`: Memindahkan rentang elemen (`move`)
- `tabel.bungkus(...)`: Membungkus argumen ke tabel (`pack`)
- `tabel.lepas(t)`: Membongkar elemen tabel (`unpack`)

#### Modul `teks` (`string`)
- `teks.panjang(s)`: Panjang karakter string (`len`)
- `teks.huruf_besar(s)`: Mengubah string menjadi huruf kapital (`upper`)
- `teks.huruf_kecil(s)`: Mengubah string menjadi huruf kecil (`lower`)
- `teks.potong(s, i, [j])`: Memotong substring (`sub`)
- `teks.cari(s, pola)`: Menemukan posisi pola dalam teks (`find`)
- `teks.ganti(s, pola, ganti)`: Mengganti teks sesuai pola regex/string (`gsub`)
- `teks.format(fmt, ...)`: Memformat teks string (`format`)
- `teks.ulang(s, n)`: Mengulang teks n kali (`rep`)
- `teks.balik(s)`: Membalik urutan karakter teks (`reverse`)

#### Modul `sistem` (`os`)
- `sistem.waktu()`: Mengambil timestamp UNIX saat ini (`time`)
- `sistem.jam()`: Waktu CPU yang digunakan program (`clock`)
- `sistem.tanggal([format])`: Format tanggal dan waktu saat ini (`date`)
- `sistem.keluar([kode])`: Menghentikan eksekusi program (`exit`)
- `sistem.jalankan(perintah)`: Menjalankan perintah sistem operasi (`execute`)
- `sistem.ambil_env(variabel)`: Mengambil nilai *environment variable* (`getenv`)
- `sistem.hapus(nama_file)`: Menghapus berkas dari disk (`remove`)
- `sistem.ganti_nama(lama, baru)`: Mengubah nama berkas (`rename`)

#### Modul `korutin` (`coroutine`)
- `korutin.buat(fungsi)`: Membuat instance korutin baru (`create`)
- `korutin.lanjut(ko, ...)`: Menjalankan / melanjutkan korutin (`resume`)
- `korutin.hasil(...)`: Menyerahkan eksekusi dari dalam korutin (`yield`)
- `korutin.status(ko)`: Memeriksa status korutin: `"suspended"`, `"running"`, `"dead"` (`status`)
- `korutin.bungkus(fungsi)`: Membuat fungsi wrapper korutin (`wrap`)

---

## Panduan Build & Kompilasi

### Persyaratan
- Kompiler C++17 modern (mendukung `clang++`, `zig c++`, `g++`, atau `cl.exe` MSVC).
- Berkas source Lua 5.4 (disediakan pada subdirektori `lua-5.4.7/src`).

### Menjalankan Build Otomatis
Pada terminal Windows (PowerShell / Command Prompt):
```bat
.\build.bat
```

### Kompilasi Manual Menggunakan Perintah CLI
```powershell
# 1. Kompilasi runtime inti Lua ke static library
pushd lua-5.4.7\src
Get-ChildItem *.c | Where-Object { $_.Name -ne 'lua.c' -and $_.Name -ne 'luac.c' } | ForEach-Object { zig cc -O2 -c $_.Name }
zig ar rcs liblua.a (Get-ChildItem *.obj | Select-Object -ExpandProperty Name)
Remove-Item *.obj
popd

# 2. Kompilasi Engine Interpreter Luai ke Executable
zig c++ -O2 -std=c++17 src/lexer.cpp src/runtime.cpp src/repl.cpp src/main.cpp lua-5.4.7/src/liblua.a -o luai.exe
```

---

## Panduan Penggunaan Interpreter

### 1. Mode Eksekusi Berkas (`.luai`)
Jalankan file kode sumber `.luai` melalui terminal dengan menyertakan argumen nama berkas:
```powershell
.\luai.exe test_luai.luai
```
Argumen tambahan yang diberikan di terminal akan diteruskan ke dalam tabel global `arg`:
```powershell
.\luai.exe skrip.luai argumen1 argumen2
```

### 2. Mode Interaktif (REPL)
Jalankan `luai.exe` tanpa argumen (atau klik dua kali berkas executable):
```powershell
.\luai.exe
```
Terminal akan membuka sesi interaktif:
```text
Luai 1.0.0 (Lua dialek Bahasa Indonesia) [Lua 5.4.7]
Ketik "keluar" atau "exit" untuk mengakhiri sesi interaktif.

luai> 25 * 4
100
luai> matematika.akar(144)
12.0
luai> lokal s = "Bahasa Luai"
luai> teks.huruf_besar(s)
BAHASA LUAI
luai> fungsi sapa(nama)
>> kembalikan "Halo, " .. nama .. "!"
>> selesai
luai> sapa("Indonesia")
Halo, Indonesia!
luai> keluar
```

### 3. Mode Eksekusi Baris Perintah Langsung (`-e`)
```powershell
.\luai.exe -e "untuk i = 1, 3 lakukan cetak('Angka:', i) selesai"
```

### 4. Bantuan dan Versi
```powershell
.\luai.exe --versi
.\luai.exe --bantuan
```

---

## Contoh Kode Program

### Contoh 1: Percabangan, Fungsi Rekursif, dan Perulangan (`test_luai.luai`)
```lua
lokal nilai_ujian = 85

jika nilai_ujian >= 90 maka
    cetak("Predikat: Sangat Memuaskan (A)")
atau_jika nilai_ujian >= 80 maka
    cetak("Predikat: Memuaskan (B)")
selain_itu
    cetak("Predikat: Cukup")
selesai

fungsi faktorial(n)
    jika n <= 1 maka
        kembalikan 1
    selesai
    kembalikan n * faktorial(n - 1)
selesai

cetak("Faktorial dari 5 adalah:", faktorial(5))

untuk i = 1, 5 lakukan
    cetak("Iterasi ke-", i, "kuadrat:", i * i)
selesai
```

### Contoh 2: Pemrograman Berorientasi Objek & Metatabel
```lua
lokal Vektor = {}
Vektor.__index = Vektor

fungsi Vektor.baru(x, y)
    lokal objek = set_metatabel({}, Vektor)
    objek.x = x
    objek.y = y
    kembalikan objek
selesai

fungsi Vektor:panjang()
    kembalikan matematika.akar(self.x^2 + self.y^2)
selesai

lokal v = Vektor.baru(3, 4)
cetak("Panjang vektor (3, 4):", v:panjang())
```

### Contoh 3: Korutin & Alur Asinkron Kooperatif
```lua
lokal k = korutin.buat(fungsi(pesan)
    cetak("Korutin dimulai:", pesan)
    korutin.hasil("Jeda pertama")
    cetak("Korutin dilanjutkan")
    kembalikan "Selesai"
selesai)

lokal status, hasil = korutin.lanjut(k, "Mulai tugas")
cetak("Hasil dari korutin:", hasil)
lokal status2, hasil2 = korutin.lanjut(k)
cetak("Hasil akhir korutin:", hasil2)
```

---

## Ekstensi Editor & IDE Tooling

### 1. Visual Studio Code ([`luai-vscode/`](file:///C:/Users/Administrator/Downloads/Luai/luai-vscode))
- **Fitur**: Syntax Highlighting, Integrated Code Runner (Tombol Play ▶️), REPL Terminal, Snippets, dan Bundled Binary Zero-Config.
- **Installer**: [`luai-vscode/luai-vscode-1.1.0.vsix`](file:///C:/Users/Administrator/Downloads/Luai/luai-vscode/luai-vscode-1.1.0.vsix)

### 2. Acode Editor untuk Android ([`luai-acode/`](file:///C:/Users/Administrator/Downloads/Luai/luai-acode))
- **Fitur**: Syntax Highlighting Ace Mode, In-App Console Runner Mandiri (Zero-Config, berjalan tanpa Termux/Root), Floating Action Button (▶️), dan Snippets.
- **Installer**: [`luai-acode/plugin.zip`](file:///C:/Users/Administrator/Downloads/Luai/luai-acode/plugin.zip) (atau [`luai-acode/luai-acode-1.0.0.zip`](file:///C:/Users/Administrator/Downloads/Luai/luai-acode/luai-acode-1.0.0.zip))

### 3. Termux Android Binary Native ([`luai-termux/`](file:///C:/Users/Administrator/Downloads/Luai/luai-termux))
- **Fitur**: Binary ELF static mandiri untuk ARM64 & x86_64 Termux di Android. Mendukung eksekusi file langsung, REPL interaktif, dan skrip instalasi otomatis.
- **Arsip**: [`luai-termux.tar.gz`](file:///C:/Users/Administrator/Downloads/Luai/luai-termux.tar.gz) / [`luai-termux.zip`](file:///C:/Users/Administrator/Downloads/Luai/luai-termux.zip)

---

## Struktur Direktori Proyek

```text
Luai/
├── src/
│   ├── lexer.hpp           # Header lexer / transpiler sintaks Bahasa Indonesia
│   ├── lexer.cpp           # Implementasi tokenisasi dan pemetaan kata kunci
│   ├── runtime.hpp         # Header runtime engine Luai & binding lingkungan
│   ├── runtime.cpp         # Implementasi jembatan Lua C API, modul, & global
│   ├── repl.hpp            # Header interaktif REPL shell
│   ├── repl.cpp            # Implementasi interaktif REPL & multiline support
│   └── main.cpp            # Titik masuk utama program CLI
├── examples/
│   ├── demo_lengkap.luai             # Demonstrasi lengkap seluruh fitur
│   ├── perulangan_dan_fungsi.luai    # Demonstrasi kontrol alur & fungsi
│   ├── struktur_data.luai            # Demonstrasi tabel & OOP metatables
│   └── korutin_dan_modul.luai        # Demonstrasi korutin & import modul
├── luai-vscode/            # Ekstensi resmi Visual Studio Code (VSIX + Bundled Runner)
│   ├── bin/luai.exe
│   ├── extension.js
│   ├── package.json
│   ├── luai-vscode-1.1.0.vsix
│   └── README.md
├── luai-acode/             # Ekstensi resmi Acode Android (Plugin Zip + In-App JS Runner)
│   ├── fengari-web.js
│   ├── luai-engine.js
│   ├── main.js
│   ├── plugin.json
│   ├── plugin.zip
│   └── readme.md
├── luai-termux/            # Paket Binary Native Termux Android (ARM64 & x86_64)
│   ├── luai
│   ├── luai-arm64
│   ├── luai-x86_64
│   ├── install.sh
│   ├── test_luai.luai
│   └── README.md
├── lua-5.4.7/              # Runtime engine fondasi Lua 5.4 C core
├── test_luai.luai          # Skrip pengujian utama
├── modul_kalkulator.luai   # Contoh modul terpisah untuk pengujian import
├── build.bat               # Skrip build otomatis
├── luai.exe                # Executable binary interpreter Windows
└── README.md               # Dokumentasi resmi bahasa pemrograman Luai
```
