/*
 * ============================================================================
 *  arm32-softfloat.c — Jembatan helper konversi integer 64-bit -> float 32-bit
 * ============================================================================
 *
 *  MASALAH
 *  -------
 *  Saat mengompilasi LuaJIT untuk target `arm-linux-musleabihf` (ARM 32-bit
 *  hard-float) dengan Zig, penautan gagal dengan pesan:
 *
 *      ld.lld: error: undefined symbol: __floatdidf
 *      ld.lld: error: undefined symbol: __floatundidf
 *      ld.lld: error: undefined symbol: __floatdisf
 *      ld.lld: error: undefined symbol: __floatundisf
 *
 *  Penyebabnya: `lj_ir.o` milik LuaJIT memanggil nama simbol "generik"
 *  tersebut, sedangkan compiler-rt yang dibundel Zig untuk target ARM 32-bit
 *  hanya menyediakan nama sesuai ABI ARM EABI:
 *      __aeabi_l2d, __aeabi_ul2d, __aeabi_l2f, __aeabi_ul2f
 *
 *  SOLUSI
 *  ------
 *  Berkas ini hanya menjadi "jembatan" dari nama generik ke helper EABI
 *  milik Zig. Pemanggilan helper EABI ditulis eksplisit (bukan lewat cast
 *  `(double)v`) supaya tidak mungkin terjadi rekursi ke fungsi itu sendiri,
 *  sehingga hasil pembulatan konversinya tetap benar.
 *
 *  Berkas ini HANYA dipakai untuk build ARM 32-bit, lihat
 *  luai-termux/build-termux.sh (fungsi build_shim / link_binary).
 * ============================================================================
 */

/* Helper EABI yang sudah disediakan compiler-rt (Zig) / libgcc. */
extern double __aeabi_l2d(long long v);              /* int64  -> double */
extern double __aeabi_ul2d(unsigned long long v);    /* uint64 -> double */
extern float  __aeabi_l2f(long long v);              /* int64  -> float  */
extern float  __aeabi_ul2f(unsigned long long v);    /* uint64 -> float  */

double __floatdidf(long long v) {
    return __aeabi_l2d(v);
}

double __floatundidf(unsigned long long v) {
    return __aeabi_ul2d(v);
}

float __floatdisf(long long v) {
    return __aeabi_l2f(v);
}

float __floatundisf(unsigned long long v) {
    return __aeabi_ul2f(v);
}
