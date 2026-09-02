# Luai Language Support & Integrated Runner for Visual Studio Code

Ekstensi resmi Visual Studio Code dan VSCodium untuk bahasa pemrograman **Luai** (dialek Bahasa Indonesia dari Lua 5.4) yang dilengkapi dengan **Syntax Highlighting**, **Snippets**, serta **Integrated Code Runner Bawaan (Zero-Config / Siap Pakai)**.

---

## Keunggulan Utama (Siap Pakai Tanpa Ribet)

1. **Zero-Configuration & Zero-Download Eksternal**:
   - Engine interpreter Luai (`luai.exe`) sudah **terintegrasi dan dibundel langsung** di dalam paket ekstensi ini.
   - Anda **tidak perlu mengunduh installer Luai terpisah** dan **tidak perlu menyetel Environment Variable `PATH`** secara manual. Setelah ekstensi terpasang, Anda bisa langsung menulis dan mengeksekusi kode `.luai`.
2. **Tombol Play ▶️ Bawaan VS Code**:
   - Tombol **Jalankan Berkas (Run File)** muncul otomatis di sudut kanan atas editor ketika Anda membuka berkas berekstensi `.luai`. Cukup satu kali klik untuk menjalankan kode di Terminal terintegrasi.
3. **Pintasan Keyboard Cepat**:
   - `Ctrl + F5`: Jalankan seluruh berkas `.luai` yang sedang aktif.
   - `Shift + F5`: Jalankan potongan kode Luai yang sedang diblok/diseleksi (*Run Selection*).
4. **Terminal Interaktif REPL Terintegrasi**:
   - Buka Command Palette (`Ctrl + Shift + P`) lalu pilih `Luai: Buka Terminal Interaktif REPL` untuk langsung bereksperimen dengan shell interaktif Luai.
5. **Pewarnaan Sintaks Lengkap (TextMate Grammar)**:
   - Mendukung 22 kata kunci resmi Bahasa Indonesia (`jika`, `maka`, `selesai`, `fungsi`, `lokal`, `untuk`, `selama`, `ulang`, dll.), modul terjemahan (`matematika`, `tabel`, `teks`, `sistem`, `io`, `korutin`), konstanta (`benar`, `salah`, `kosong`, `nihil`), string, komentar, dan operator.
6. **Kumpulan Snippets & Language Tools**:
   - Auto-closing kurung `{ }`, `[ ]`, `( )`, tanda kutip `" "`, `' '`, dan blok multiline `[[ ]]`.
   - Toggle comment otomatis dengan `Ctrl + /` (menghasilkan `--`).
   - Snippets instan untuk fungsi, percabangan, perulangan, dan kelas OOP.

---

## Cara Menjalankan Kode Luai di VS Code

### 1. Menggunakan Tombol Play (▶️)
Buka berkas `.luai` apa saja, lalu klik tombol **Play** di bilah judul editor (kanan atas).

### 2. Menggunakan Pintasan Keyboard
- Tekan `Ctrl + F5` pada editor berkas `.luai`.

### 3. Menggunakan Menu Klik Kanan (Context Menu)
- Klik kanan di dalam editor atau di berkas file explorer, lalu pilih **Luai: Jalankan Berkas (Run File)**.
- Jika ingin menjalankan sebagian baris saja, blok baris kode tersebut, klik kanan, lalu pilih **Luai: Jalankan Kode Terpilih (Run Selection)** (atau `Shift + F5`).

---

## Pengaturan Ekstensi (Settings)

Buka menu *Settings* (`Ctrl + ,`) dan cari `Luai` untuk menyesuaikan opsi berikut:

| Pengaturan | Tipe | Nilai Bawaan | Deskripsi |
|:---|:---|:---|:---|
| `luai.executablePath` | String | `""` *(kosong)* | Jalur kustom ke binary `luai.exe`. Jika dikosongkan, ekstensi otomatis memakai binary bawaan di dalam ekstensi. |
| `luai.clearTerminalBeforeRun` | Boolean | `true` | Membersihkan layar terminal sebelum menjalankan kode baru. |

---

## Panduan Instalasi Ekstensi

### Metode 1: Menggunakan Berkas Paket `.vsix`

#### A. Melalui Terminal / CLI
```bash
code --install-extension luai-vscode-1.1.0.vsix
```
*Untuk pengguna VSCodium:*
```bash
codium --install-extension luai-vscode-1.1.0.vsix
```

#### B. Melalui Antarmuka Visual Studio Code (GUI)
1. Buka VS Code.
2. Buka tab **Extensions** di panel kiri (`Ctrl + Shift + X`).
3. Klik ikon titik tiga (**...**) di sudut kanan atas panel Extensions.
4. Pilih menu **Install from VSIX...**.
5. Pilih berkas `luai-vscode-1.1.0.vsix`.

---

### Metode 2: Instalasi Manual (Salin Folder)

Salin folder `luai-vscode` langsung ke direktori ekstensi pengguna:

- **Windows (PowerShell)**:
  ```powershell
  Copy-Item -Recurse -Force luai-vscode "$env:USERPROFILE\.vscode\extensions\luai-vscode"
  ```
- **Linux / macOS**:
  ```bash
  cp -r luai-vscode ~/.vscode/extensions/
  ```

Muat ulang jendela VS Code (*Developer: Reload Window* atau `Ctrl + Shift + P`).

---

## Contoh Kode Program Luai

```lua
lokal nama = "Pengembang Luai"
lokal angka = {10, 25, 4, 88, 12}

fungsi hitung_total(daftar)
    lokal total = 0
    untuk _, v dalam i_pasangan(daftar) lakukan
        total = total + v
    selesai
    kembalikan total
selesai

cetak("Halo, " .. nama .. "!")
lokal hasil = hitung_total(angka)
cetak("Total nilai array:", hasil)
cetak("Akar kuadrat total:", matematika.akar(hasil))
```
