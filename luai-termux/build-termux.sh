#!/usr/bin/env bash
# =============================================================================
#  build-termux.sh — Bangun binary Luai siap pakai untuk Android / Termux
# =============================================================================
#
#  KENAPA ADA SKRIP INI?
#  ---------------------
#  Binary yang dikompilasi memakai glibc (misalnya `gcc-aarch64-linux-gnu`,
#  atau `g++ -static` milik Ubuntu) memanggil syscall `rseq` dan `clone3`
#  saat start-up. Filter seccomp milik Android memblokir syscall tersebut,
#  sehingga di Termux muncul pesan:
#
#      Bad system call
#
#  (proses dibunuh oleh sinyal SIGSYS / exit code 159).
#
#  Solusinya: bangun binary secara FULL STATIC memakai libc **musl**
#  (lewat Zig sebagai cross-compiler). musl tidak pernah memanggil rseq
#  maupun clone3, jadi binary aman dijalankan di Android/Termux.
#
#  ENGINE
#  ------
#    1. LuaJIT  (JIT Compiler tetap AKTIF)  -> engine utama
#    2. Lua 5.4 (interpreter, tanpa JIT)    -> cadangan otomatis kalau (1) gagal
#
#  PEMAKAIAN
#  ---------
#    bash luai-termux/build-termux.sh <arm64|arm32|x86_64> [dir-keluaran]
#
#  VARIABEL LINGKUNGAN (opsional)
#  ------------------------------
#    ZIG        perintah zig yang dipakai            (default: zig)
#    HOST_CC    kompiler host untuk alat LuaJIT      (default: cc, ARM32: "cc -m32")
#    LUA54_DIR  folder "<...>/lua-5.4.7/src"         (sumber engine cadangan)
#    ENGINE     auto (default) | luajit | lua54
#    JOBS       jumlah job paralel make              (default: jumlah CPU)
# =============================================================================

set -u

ARCH="${1:-}"
OUTDIR="${2:-.}"

ZIG="${ZIG:-zig}"
ENGINE="${ENGINE:-auto}"
LUA54_DIR="${LUA54_DIR:-}"
JOBS="${JOBS:-}"

if [ -z "$JOBS" ]; then
    JOBS="$( (nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || echo 2) )"
fi

case "$ARCH" in
    arm64)  TRIPLE="aarch64-linux-musl";   BIN_NAME="luai-arm64"  ;;
    arm32)  TRIPLE="arm-linux-musleabihf"; BIN_NAME="luai-arm32"  ;;
    x86_64) TRIPLE="x86_64-linux-musl";    BIN_NAME="luai-x86_64" ;;
    *)
        echo "KESALAHAN: arsitektur '$ARCH' tidak dikenal (pakai: arm64, arm32, x86_64)" >&2
        exit 2
        ;;
esac

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC_DIR="$ROOT_DIR/src"
LJ_DIR="$ROOT_DIR/luajit/src"
SHIM_SRC="$ROOT_DIR/luai-termux/arm32-softfloat.c"
WORK_DIR="$ROOT_DIR/build/termux-$ARCH"
LUA54_OBJ_DIR="$WORK_DIR/lua54-obj"
OUT_FILE="$OUTDIR/$BIN_NAME"
ENGINE_USED=""

if [ -z "${HOST_CC:-}" ]; then
    if [ "$ARCH" = "arm32" ]; then
        # Alat bantu build LuaJIT harus 32-bit agar ukuran pointer-nya sama
        # dengan target ARM 32-bit (kalau tidak: "pointer size mismatch").
        HOST_CC="cc -m32"
    else
        HOST_CC="cc"
    fi
fi

mkdir -p "$WORK_DIR" "$OUTDIR"
rm -f "$OUT_FILE"
: > "$WORK_DIR/lua54-build.log"
: > "$WORK_DIR/shim-build.log"

say() {
    echo "[build-termux][$ARCH] $*"
}

warn() {
    if [ -n "${GITHUB_ACTIONS:-}" ]; then
        echo "::warning title=build-termux ($ARCH)::$*"
    fi
    echo "[build-termux][$ARCH][PERINGATAN] $*" >&2
}

fail() {
    echo "KESALAHAN: $*" >&2
    exit 1
}

command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# -----------------------------------------------------------------------------
# 1. Cek kompiler host (dipakai hanya untuk membangun alat bantu LuaJIT)
# -----------------------------------------------------------------------------
host_cc_usable() {
    printf 'int main(void){return 0;}\n' > "$WORK_DIR/_hostcheck.c"
    # $HOST_CC sengaja tidak dikutip agar "cc -m32" terpecah menjadi dua argumen.
    if $HOST_CC "$WORK_DIR/_hostcheck.c" -o "$WORK_DIR/_hostcheck" \
            > "$WORK_DIR/_hostcheck.log" 2>&1; then
        return 0
    fi
    warn "kompiler host '$HOST_CC' tidak bisa dipakai (lihat $WORK_DIR/_hostcheck.log)"
    return 1
}

# -----------------------------------------------------------------------------
# 2. Bangun LuaJIT static untuk target (JIT Compiler aktif)
# -----------------------------------------------------------------------------
build_luajit_lib() {
    say "membangun LuaJIT static untuk $TRIPLE (JIT aktif) ..."
    ( cd "$LJ_DIR" && make clean ) >/dev/null 2>&1 || true
    if ! make -C "$LJ_DIR" -j"$JOBS" \
                HOST_CC="$HOST_CC" \
                CC="$ZIG cc -target $TRIPLE" \
                TARGET_SYS=Linux \
                BUILDMODE=static \
                XCFLAGS="-DLUAJIT_ENABLE_LUA52COMPAT" \
                libluajit.a > "$WORK_DIR/luajit-build.log" 2>&1; then
        warn "build LuaJIT gagal (log: $WORK_DIR/luajit-build.log)"
        return 1
    fi
    [ -f "$LJ_DIR/libluajit.a" ] || { warn "libluajit.a tidak dihasilkan"; return 1; }
    return 0
}

# -----------------------------------------------------------------------------
# 3. Bangun engine cadangan Lua 5.4 (dipakai hanya bila LuaJIT gagal)
# -----------------------------------------------------------------------------
prepare_lua54() {
    if [ -n "$LUA54_DIR" ] && [ -f "$LUA54_DIR/lua.h" ]; then
        LUA54_SRC="$LUA54_DIR"
        return 0
    fi

    local cand
    for cand in "$ROOT_DIR"/build/lua-5.4.*/src "$WORK_DIR"/lua-5.4.*/src; do
        if [ -f "$cand/lua.h" ]; then
            LUA54_SRC="$cand"
            return 0
        fi
    done

    LUA54_SRC="$WORK_DIR/lua-5.4.7/src"
    if [ ! -f "$LUA54_SRC/lua.h" ]; then
        say "mengunduh sumber Lua 5.4.7 (engine cadangan) ..."
        curl -fsSL "https://www.lua.org/ftp/lua-5.4.7.tar.gz" \
            -o "$WORK_DIR/lua-5.4.7.tar.gz" || { warn "gagal mengunduh Lua 5.4.7"; return 1; }
        tar -xzf "$WORK_DIR/lua-5.4.7.tar.gz" -C "$WORK_DIR" \
            || { warn "gagal mengekstrak Lua 5.4.7"; return 1; }
    fi

    [ -f "$LUA54_SRC/lua.h" ]
}

build_lua54_objects() {
    prepare_lua54 || return 1
    say "membangun Lua 5.4 static untuk $TRIPLE ..."
    rm -rf "$LUA54_OBJ_DIR"
    mkdir -p "$LUA54_OBJ_DIR"

    local f base out
    for f in "$LUA54_SRC"/*.c; do
        base="$(basename "$f")"
        case "$base" in
            lua.c|luac.c|onelua.c) continue ;;
        esac
        out="$LUA54_OBJ_DIR/${base%.c}.o"
        if ! "$ZIG" cc -target "$TRIPLE" -O2 -DLUA_USE_POSIX -I"$LUA54_SRC" \
                    -c "$f" -o "$out" >> "$WORK_DIR/lua54-build.log" 2>&1; then
            warn "gagal mengompilasi $base (log: $WORK_DIR/lua54-build.log)"
            return 1
        fi
    done
    return 0
}

# -----------------------------------------------------------------------------
# 4. Helper soft-float khusus ARM 32-bit (lihat luai-termux/arm32-softfloat.c)
# -----------------------------------------------------------------------------
build_shim() {
    if [ "$ARCH" != "arm32" ]; then
        return 0
    fi
    say "mengompilasi helper soft-float ARM32 ..."
    if ! "$ZIG" cc -target "$TRIPLE" -O2 -c "$SHIM_SRC" \
                -o "$WORK_DIR/arm32-softfloat.o" >> "$WORK_DIR/shim-build.log" 2>&1; then
        warn "gagal mengompilasi helper soft-float ARM32 (log: $WORK_DIR/shim-build.log)"
        return 1
    fi
    return 0
}

# -----------------------------------------------------------------------------
# 5. Tautkan binary Luai (static musl)
# -----------------------------------------------------------------------------
link_binary() {
    local engine="$1"
    local label
    local -a args=() objs=() libs=()

    if [ "$engine" = "luajit" ]; then
        label="LuaJIT"
    else
        label="Lua 5.4"
    fi

    args+=("-DLUAI_BUILD_PLATFORM=\"$TRIPLE (musl static, $label)\"")
    objs+=("$SRC_DIR/lexer.cpp" "$SRC_DIR/runtime.cpp" "$SRC_DIR/repl.cpp" "$SRC_DIR/json_module.cpp" "$SRC_DIR/http_module.cpp" "$SRC_DIR/main.cpp")

    if [ "$engine" = "luajit" ]; then
        args+=(-I"$LJ_DIR")
        libs+=("$LJ_DIR/libluajit.a")
    else
        args+=(-DUSE_LUA54 -I"$LUA54_SRC")
        libs+=("$LUA54_OBJ_DIR"/*.o)
    fi

    if [ "$ARCH" = "arm32" ]; then
        libs+=("$WORK_DIR/arm32-softfloat.o")
    fi

    say "menautkan binary: $OUT_FILE"
    if ! "$ZIG" c++ -target "$TRIPLE" -static -O2 -std=c++17 -Wl,-s \
                "${args[@]}" "${objs[@]}" "${libs[@]}" -o "$OUT_FILE" \
                > "$WORK_DIR/link-$engine.log" 2>&1; then
        warn "penautan gagal dengan engine $engine (log: $WORK_DIR/link-$engine.log)"
        return 1
    fi
    [ -f "$OUT_FILE" ]
}

# -----------------------------------------------------------------------------
# 6. Alur utama
# -----------------------------------------------------------------------------
ZIG_VERSION_TEXT="$("$ZIG" version 2>/dev/null || echo '?')"
say "target  : $TRIPLE -> $OUT_FILE"
say "engine  : $ENGINE | zig: $ZIG_VERSION_TEXT | host-cc: $HOST_CC | cpu: $JOBS"

if [ "$ENGINE" != "lua54" ]; then
    if host_cc_usable; then
        if build_luajit_lib && build_shim && link_binary luajit; then
            ENGINE_USED="LuaJIT (JIT aktif)"
        fi
    else
        warn "melewati LuaJIT karena kompiler host tidak siap"
    fi
fi

if [ -z "$ENGINE_USED" ] && [ "$ENGINE" != "luajit" ]; then
    warn "LuaJIT tidak bisa dipakai untuk $ARCH -> memakai engine cadangan Lua 5.4 (JIT tidak aktif)"
    if build_lua54_objects && link_binary lua54; then
        ENGINE_USED="Lua 5.4 (cadangan, tanpa JIT)"
    fi
fi

if [ -z "$ENGINE_USED" ] || [ ! -f "$OUT_FILE" ]; then
    fail "gagal membangun $BIN_NAME untuk arsitektur $ARCH"
fi

# -----------------------------------------------------------------------------
# 7. Pemeriksaan akhir (harus static & bukan glibc)
# -----------------------------------------------------------------------------
if command_exists file; then
    file "$OUT_FILE" | tee "$WORK_DIR/file-info.txt"
    if ! grep -q "statically linked" "$WORK_DIR/file-info.txt"; then
        fail "$BIN_NAME bukan binary static (berisiko gagal di Android)"
    fi
fi

if command_exists readelf; then
    # Binary glibc punya catatan NT_GNU_ABI_TAG di program header. Bila ada,
    # binary bakal memanggil rseq/clone3 dan mati dengan "Bad system call".
    if readelf -n "$OUT_FILE" 2>/dev/null | grep -qi "ABI version tag"; then
        fail "$BIN_NAME terdeteksi menautkan glibc (penyebab 'Bad system call' di Termux)"
    fi
fi

say "SELESAI: $OUT_FILE"
say "engine  : $ENGINE_USED"
say "ukuran  : $(wc -c < "$OUT_FILE" | tr -d ' ') byte"
exit 0
