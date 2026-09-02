#!/data/data/com.termux/files/usr/bin/bash
ARCH=$(uname -m)
TARGET_BIN="luai"
if [ "$ARCH" = "aarch64" ] || [ "$ARCH" = "arm64" ]; then
    if [ -f "luai-arm64" ]; then
        TARGET_BIN="luai-arm64"
    fi
elif [ "$ARCH" = "x86_64" ]; then
    if [ -f "luai-x86_64" ]; then
        TARGET_BIN="luai-x86_64"
    fi
fi

cp "$TARGET_BIN" "$PREFIX/bin/luai"
chmod +x "$PREFIX/bin/luai"
echo "==========================================="
echo "Luai berhasil dipasang ke Termux!"
echo "Lokasi binary: $PREFIX/bin/luai"
echo "==========================================="
echo "Penggunaan:"
echo "  luai             -> Buka interactive REPL"
echo "  luai skrip.luai  -> Eksekusi berkas Luai"
echo "==========================================="
