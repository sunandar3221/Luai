#!/data/data/com.termux/files/usr/bin/bash
# ==========================================================
#  Installer Pintar Luai untuk Android Termux
#  Mendukung otomatis: ARM64 (64-bit), ARM32 (32-bit), x86_64
# ==========================================================

set -e

# Tentukan direktori PREFIX Termux
if [ -z "$PREFIX" ]; then
    if [ -d "/data/data/com.termux/files/usr" ]; then
        PREFIX="/data/data/com.termux/files/usr"
    else
        PREFIX="/usr/local"
    fi
fi

BIN_DIR="$PREFIX/bin"
mkdir -p "$BIN_DIR"
TARGET_BIN="$BIN_DIR/luai"

echo "=========================================================="
echo "    Memulai Pemasangan Bahasa Pemrograman Luai (Termux)    "
echo "=========================================================="

# Deteksi arsitektur CPU perangkat secara otomatis
RAW_ARCH=$(uname -m)
echo "[1/3] Mendeteksi arsitektur perangkat: $RAW_ARCH"

BIN_NAME="luai-arm64"
ARCH_DESC="ARM 64-bit (aarch64)"

case "$RAW_ARCH" in
    aarch64|arm64)
        BIN_NAME="luai-arm64"
        ARCH_DESC="ARM 64-bit (aarch64)"
        ;;
    armv7*|armv8l|armhf|arm|armeabi*|armv7a)
        BIN_NAME="luai-arm32"
        ARCH_DESC="ARM 32-bit (armv7l / 32-bit)"
        ;;
    x86_64|amd64)
        BIN_NAME="luai-x86_64"
        ARCH_DESC="x86_64 (Intel/AMD 64-bit)"
        ;;
    i686|i386)
        echo "Peringatan: Perangkat terdeteksi 32-bit x86. Mencoba x86_64..."
        BIN_NAME="luai-x86_64"
        ARCH_DESC="x86"
        ;;
    *)
        echo "Arsitektur '$RAW_ARCH' tidak umum, menggunakan ARM64 sebagai bawaan."
        BIN_NAME="luai-arm64"
        ARCH_DESC="ARM 64-bit (bawaan)"
        ;;
esac

echo "      Target binary terpilih: $BIN_NAME ($ARCH_DESC)"

# Lokasi skrip saat ini jika dijalankan lokal
SCRIPT_DIR=""
if [ -n "${BASH_SOURCE[0]}" ] && [ -f "${BASH_SOURCE[0]}" ]; then
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
fi

echo "[2/3] Mempersiapkan binary Luai..."

INSTALLED=0

# Cek apakah berkas ada di direktori lokal
if [ -n "$SCRIPT_DIR" ] && [ -f "$SCRIPT_DIR/$BIN_NAME" ]; then
    echo "      Menggunakan berkas lokal: $SCRIPT_DIR/$BIN_NAME"
    cp -f "$SCRIPT_DIR/$BIN_NAME" "$TARGET_BIN"
    INSTALLED=1
elif [ -f "./$BIN_NAME" ]; then
    echo "      Menggunakan berkas lokal: ./$BIN_NAME"
    cp -f "./$BIN_NAME" "$TARGET_BIN"
    INSTALLED=1
elif [ -n "$SCRIPT_DIR" ] && [ -f "$SCRIPT_DIR/luai" ]; then
    echo "      Menggunakan berkas lokal: $SCRIPT_DIR/luai"
    cp -f "$SCRIPT_DIR/luai" "$TARGET_BIN"
    INSTALLED=1
fi

# Jika tidak ada berkas lokal (misal via curl | bash), unduh dari GitHub Releases
if [ "$INSTALLED" -eq 0 ]; then
    echo "      Mengunduh binary '$BIN_NAME' dari rilis GitHub..."
    URL_RELEASE="https://github.com/sunandar3221/Luai/releases/download/v1.0.0/$BIN_NAME"
    URL_RAW="https://raw.githubusercontent.com/sunandar3221/Luai/main/luai-termux/$BIN_NAME"

    if curl -sSL --fail "$URL_RELEASE" -o "$TARGET_BIN" 2>/dev/null; then
        echo "      Berhasil mengunduh dari GitHub Releases!"
        INSTALLED=1
    elif curl -sSL --fail "$URL_RAW" -o "$TARGET_BIN" 2>/dev/null; then
        echo "      Berhasil mengunduh dari repositori utama!"
        INSTALLED=1
    else
        echo "      [Peringatan] Gagal mengunduh $BIN_NAME. Mencoba unduh luai standar..."
        curl -sSL "https://github.com/sunandar3221/Luai/releases/download/v1.0.0/luai" -o "$TARGET_BIN" 2>/dev/null || \
        curl -sSL "https://raw.githubusercontent.com/sunandar3221/Luai/main/luai-termux/luai" -o "$TARGET_BIN"
        INSTALLED=1
    fi
fi

chmod +x "$TARGET_BIN"

echo "[3/3] Memverifikasi instalasi..."
if "$TARGET_BIN" -e "cetak('Verifikasi Luai: SUKSES')" 2>/dev/null; then
    echo "      Status eksekusi: Berfungsi normal!"
else
    echo "      Catatan: Binary terpasang di $TARGET_BIN."
fi

echo "=========================================================="
echo " [SUKSES] Luai berhasil dipasang ke Termux Android!"
echo " Lokasi binary: $TARGET_BIN"
echo " Arsitektur   : $ARCH_DESC"
echo "=========================================================="
echo " Cara Penggunaan:"
echo "   luai               -> Membuka REPL interaktif"
echo "   luai skrip.luai    -> Menjalankan berkas skrip Luai"
echo "   luai -e \"cetak('Halo Luai')\" -> Evaluasi satu baris kode"
echo "=========================================================="
