#!/data/data/com.termux/files/usr/bin/bash
ARCH=$(uname -m)
BIN_NAME="luai-arm64"
if [ "$ARCH" = "x86_64" ]; then
    BIN_NAME="luai-x86_64"
fi

if [ -f "$BIN_NAME" ]; then
    cp "$BIN_NAME" "$PREFIX/bin/luai"
elif [ -f "luai" ]; then
    cp "luai" "$PREFIX/bin/luai"
else
    echo "Mengunduh binary Luai untuk arsitektur $ARCH..."
    curl -sL "https://raw.githubusercontent.com/sunandar3221/Luai/main/luai-termux/$BIN_NAME" -o "$PREFIX/bin/luai" || \
    curl -sL "https://github.com/sunandar3221/Luai/releases/download/v1.0.0/$BIN_NAME" -o "$PREFIX/bin/luai"
fi

chmod +x "$PREFIX/bin/luai"
echo "==========================================="
echo "Luai berhasil dipasang ke Termux!"
echo "Lokasi binary: $PREFIX/bin/luai"
echo "==========================================="
echo "Penggunaan:"
echo "  luai             -> Buka interactive REPL"
echo "  luai skrip.luai  -> Eksekusi berkas Luai"
echo "==========================================="
