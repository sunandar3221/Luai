#ifndef LUAI_ERROR_HANDLER_HPP
#define LUAI_ERROR_HANDLER_HPP

#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace Luai {

struct ErrorLocation {
    std::string source;
    int line = 0;
    std::string message;
    bool hasLocation = false;
};

class ErrorHandler {
public:
    static std::string format(const std::string& raw) {
        if (raw.empty()) {
            return raw;
        }

        // Jangan format ulang jika pesan error sudah berformat Luai
        if (raw.rfind("[Kesalahan", 0) == 0) {
            return raw;
        }

        ErrorLocation loc = parseLocation(raw);
        std::string msg = loc.message;

        // Bersihkan whitespace di awal/akhir
        trim(msg);

        std::string category = "[Kesalahan Runtime]";
        std::string explanation;
        std::string tips;

        // 1. Parsing Kesalahan Sintaks Kompilasi
        if (contains(msg, "'then' expected")) {
            category = "[Kesalahan Sintaks]";
            std::string nearSym = extractNear(msg);
            if (!nearSym.empty() && nearSym != "<eof>") {
                explanation = "Diharapkan kata kunci 'maka' setelah kondisi percabangan 'jika', namun ditemukan '" + nearSym + "'.";
            } else {
                explanation = "Diharapkan kata kunci 'maka' setelah kondisi percabangan 'jika'.";
            }
            tips = "Format percabangan di Luai adalah: jika <kondisi> maka ... selesai";
        }
        else if (contains(msg, "'do' expected")) {
            category = "[Kesalahan Sintaks]";
            std::string nearSym = extractNear(msg);
            if (!nearSym.empty() && nearSym != "<eof>") {
                explanation = "Diharapkan kata kunci 'lakukan' pada blok perulangan, namun ditemukan '" + nearSym + "'.";
            } else {
                explanation = "Diharapkan kata kunci 'lakukan' pada blok perulangan.";
            }
            tips = "Format perulangan di Luai adalah: untuk ... lakukan ... selesai atau selama <kondisi> lakukan ... selesai";
        }
        else if (contains(msg, "'end' expected (to close '")) {
            category = "[Kesalahan Sintaks]";
            std::string blockName = extractBetween(msg, "'end' expected (to close '", "'");
            std::string atLine = extractBetween(msg, "at line ", ")");
            std::string nearSym = extractNear(msg);
            std::string blockId = translateKeyword(blockName);
            explanation = "Blok '" + blockId + "' (yang dibuka pada baris " + (atLine.empty() ? "?" : atLine) + ") belum ditutup dengan kata kunci 'selesai'.";
            if (!nearSym.empty() && nearSym != "<eof>") {
                explanation += " Ditemukan '" + nearSym + "' sebelum blok selesai.";
            }
            tips = "Tambahkan kata kunci 'selesai' untuk menutup blok '" + blockId + "' tersebut.";
        }
        else if (contains(msg, "'end' expected")) {
            category = "[Kesalahan Sintaks]";
            std::string nearSym = extractNear(msg);
            if (nearSym == "<eof>") {
                explanation = "Blok kode belum ditutup. Diharapkan kata kunci penutup 'selesai' sebelum akhir berkas/kode.";
            } else if (!nearSym.empty()) {
                explanation = "Diharapkan kata kunci penutup 'selesai' di dekat '" + nearSym + "'.";
            } else {
                explanation = "Blok kode belum ditutup. Diharapkan kata kunci 'selesai'.";
            }
            tips = "Pastikan semua blok percabangan ('jika'), perulangan ('untuk', 'selama'), dan 'fungsi' sudah ditutup dengan kata 'selesai'.";
        }
        else if (contains(msg, "'until' expected")) {
            category = "[Kesalahan Sintaks]";
            std::string nearSym = extractNear(msg);
            explanation = "Diharapkan kata kunci 'sampai' untuk mengakhiri perulangan 'ulang'.";
            if (!nearSym.empty() && nearSym != "<eof>") {
                explanation += " Ditemukan '" + nearSym + "'.";
            }
            tips = "Format perulangan di Luai adalah: ulang ... sampai <kondisi>";
        }
        else if (contains(msg, "'in' expected")) {
            category = "[Kesalahan Sintaks]";
            explanation = "Diharapkan kata kunci 'dalam' pada perulangan 'untuk'.";
            tips = "Format perulangan pasangan di Luai adalah: untuk k, v dalam pasangan(tabel) lakukan ... selesai";
        }
        else if (contains(msg, "unfinished string")) {
            category = "[Kesalahan Sintaks]";
            std::string nearSym = extractNear(msg);
            explanation = "Teks string belum ditutup.";
            if (!nearSym.empty()) {
                explanation += " Terjadi di dekat '" + nearSym + "'.";
            }
            tips = "Pastikan tanda kutip pembuka (baik ' maupun \") memiliki pasangan tanda kutip penutup yang cocok pada baris yang sama.";
        }
        else if (contains(msg, "unexpected symbol near")) {
            category = "[Kesalahan Sintaks]";
            std::string nearSym = extractNear(msg);
            explanation = "Ditemukan simbol atau karakter tak terduga '" + nearSym + "' yang tidak dikenali sintaks Luai.";
            tips = "Periksa apakah ada tanda baca yang salah ketik, tanda kurung yang belum ditutup, atau operator yang kurang.";
        }
        else if (contains(msg, "'=' expected")) {
            category = "[Kesalahan Sintaks]";
            explanation = "Diharapkan tanda sama dengan '=' untuk pemberian nilai (assignment).";
            tips = "Contoh penulisan: lokal nama = \"Budi\"";
        }
        else if (contains(msg, "'(' expected")) {
            category = "[Kesalahan Sintaks]";
            explanation = "Diharapkan tanda kurung buka '('.";
            tips = "Periksa kembali sintaks pemanggilan fungsi atau pengelompokan ekspresi.";
        }
        else if (contains(msg, "')' expected")) {
            category = "[Kesalahan Sintaks]";
            explanation = "Diharapkan tanda kurung tutup ')'.";
            tips = "Pastikan setiap tanda kurung buka '(' sudah memiliki pasangan penutup ')'.";
        }
        else if (contains(msg, "']' expected")) {
            category = "[Kesalahan Sintaks]";
            explanation = "Diharapkan tanda kurung siku penutup ']'.";
            tips = "Pastikan pengindeksan tabel tabel[...] ditutup dengan tanda ']'.";
        }
        else if (contains(msg, "'}' expected")) {
            category = "[Kesalahan Sintaks]";
            explanation = "Diharapkan tanda kurung kurawal penutup '}' untuk menutup definisi tabel.";
            tips = "Pastikan definisi tabel { ... } diakhiri dengan tanda '}'.";
        }
        else if (contains(msg, "<name> expected")) {
            category = "[Kesalahan Sintaks]";
            std::string nearSym = extractNear(msg);
            explanation = "Diharapkan nama variabel atau fungsi yang valid.";
            if (!nearSym.empty()) {
                explanation += " Ditemukan '" + nearSym + "'.";
            }
            tips = "Nama variabel dan fungsi harus diawali dengan huruf atau garis bawah (_), bukan angka atau simbol khusus.";
        }
        else if (contains(msg, "no loop to break")) {
            category = "[Kesalahan Sintaks]";
            explanation = "Perintah 'berhenti' (break) hanya boleh digunakan di dalam blok perulangan ('untuk', 'selama', atau 'ulang').";
            tips = "Hapus perintah 'berhenti' jika kode ini tidak berada di dalam blok perulangan.";
        }
        else if (contains(msg, "invalid escape sequence")) {
            category = "[Kesalahan Sintaks]";
            explanation = "Karakter escape (tanda garis miring terbalik '\\') pada teks string tidak valid.";
            tips = "Gunakan escape sequence yang sah seperti \\n (baris baru), \\t (tab), atau \\\\.";
        }

        // 2. Runtime: Indexing nil atau bukan tabel
        else if (contains(msg, "attempt to index local '")) {
            category = "[Kesalahan Runtime]";
            std::string varName = extractBetween(msg, "attempt to index local '", "'");
            explanation = "Mencoba mengakses properti/indeks dari variabel lokal '" + varName + "' yang bernilai 'nihil' (kosong).";
            tips = "Variabel '" + varName + "' belum memiliki isi tabel. Berikan nilai tabel terlebih dahulu (contoh: lokal " + varName + " = {}) sebelum mengakses isinya.";
        }
        else if (contains(msg, "attempt to index global '")) {
            category = "[Kesalahan Runtime]";
            std::string varName = extractBetween(msg, "attempt to index global '", "'");
            explanation = "Mencoba mengakses properti/indeks dari variabel global '" + varName + "' yang bernilai 'nihil' (belum dibuat/didefinisikan).";
            tips = "Variabel atau modul '" + varName + "' tidak ditemukan. Pastikan nama variabel benar atau modul sudah dimuat dengan 'butuh(\"" + varName + "\")'.";
        }
        else if (contains(msg, "attempt to index field '")) {
            category = "[Kesalahan Runtime]";
            std::string fieldName = extractBetween(msg, "attempt to index field '", "'");
            explanation = "Mencoba mengakses sub-properti dari kolom/field '" + fieldName + "' yang bernilai 'nihil' (kosong).";
            tips = "Tabel tidak memiliki kolom bernama '" + fieldName + "' atau kolom tersebut bernilai kosong.";
        }
        else if (contains(msg, "attempt to index upvalue '")) {
            category = "[Kesalahan Runtime]";
            std::string varName = extractBetween(msg, "attempt to index upvalue '", "'");
            explanation = "Mencoba mengakses properti dari variabel '" + varName + "' yang bernilai 'nihil' (kosong).";
            tips = "Pastikan variabel '" + varName + "' di lingkup luar fungsi sudah diberi nilai tabel.";
        }
        else if (contains(msg, "attempt to index a nil value")) {
            category = "[Kesalahan Runtime]";
            explanation = "Mencoba mengakses properti atau indeks dari sebuah nilai yang bernilai 'nihil' (kosong).";
            tips = "Objek yang ingin Anda akses belum memiliki nilai (bernilai nihil). Pastikan variabel sudah diisi tabel/objek.";
        }
        else if (contains(msg, "attempt to index a ") && contains(msg, " value")) {
            category = "[Kesalahan Tipe Data]";
            std::string typeStr = extractBetween(msg, "attempt to index a ", " value");
            explanation = "Mencoba mengakses indeks atau properti dari tipe data " + translateTypeName(typeStr) + ".";
            tips = "Hanya tabel dan modul yang memiliki properti. Tipe " + translateTypeName(typeStr) + " tidak memiliki indeks atau properti.";
        }

        // 3. Runtime: Pemanggilan fungsi bernilai nil atau bukan fungsi
        else if (contains(msg, "attempt to call local '")) {
            category = "[Kesalahan Runtime]";
            std::string fnName = extractBetween(msg, "attempt to call local '", "'");
            explanation = "Mencoba memanggil variabel lokal '" + fnName + "' sebagai fungsi, namun bernilai 'nihil' (bukan fungsi).";
            tips = "Fungsi lokal '" + fnName + "' belum dibuat atau belum didefinisikan sebelum baris pemanggilan ini.";
        }
        else if (contains(msg, "attempt to call global '")) {
            category = "[Kesalahan Runtime]";
            std::string fnName = extractBetween(msg, "attempt to call global '", "'");
            explanation = "Mencoba memanggil fungsi global '" + fnName + "', namun fungsi tersebut tidak ditemukan (bernilai 'nihil').";
            tips = "Periksa penulisan nama fungsi '" + fnName + "'. Pastikan ejaan huruf besar/kecil sesuai dan fungsi sudah dibuat sebelum dipanggil.";
        }
        else if (contains(msg, "attempt to call field '")) {
            category = "[Kesalahan Runtime]";
            std::string fnName = extractBetween(msg, "attempt to call field '", "'");
            explanation = "Mencoba memanggil fungsi '" + fnName + "' pada modul/tabel, namun fungsi tersebut tidak ada (bernilai 'nihil').";
            tips = "Modul atau tabel tidak memiliki fungsi bernama '" + fnName + "'. Periksa kembali daftar fungsi pada modul tersebut.";
        }
        else if (contains(msg, "attempt to call method '")) {
            category = "[Kesalahan Runtime]";
            std::string mName = extractBetween(msg, "attempt to call method '", "'");
            explanation = "Mencoba memanggil metode '" + mName + "' pada objek, namun metode tersebut tidak ditemukan (bernilai 'nihil').";
            tips = "Periksa penulisan nama metode '" + mName + "'. Misalnya pada objek berkas gunakan metode seperti 'f:tulis(...)', 'f:baca(...)', dll.";
        }
        else if (contains(msg, "attempt to call a nil value")) {
            category = "[Kesalahan Runtime]";
            explanation = "Mencoba memanggil nilai 'nihil' (kosong) sebagai fungsi.";
            tips = "Ekspresi atau variabel yang dipanggil dengan tanda kurung () ternyata kosong atau belum didefinisikan sebagai fungsi.";
        }
        else if (contains(msg, "attempt to call a ") && contains(msg, " value")) {
            category = "[Kesalahan Tipe Data]";
            std::string typeStr = extractBetween(msg, "attempt to call a ", " value");
            explanation = "Mencoba memanggil nilai bertipe " + translateTypeName(typeStr) + " seolah-olah sebuah fungsi.";
            tips = "Nilai tersebut bukan sebuah fungsi (bertipe " + translateTypeName(typeStr) + "). Jangan gunakan tanda kurung () pada nilai ini.";
        }

        // 4. Runtime: Operasi Matematika
        else if (contains(msg, "attempt to perform arithmetic on")) {
            if (contains(msg, "a string value")) {
                category = "[Kesalahan Tipe Data]";
                explanation = "Operasi matematika gagal karena nilai berupa teks (string) dan tidak dapat diubah menjadi angka secara otomatis.";
                tips = "Gunakan fungsi 'ke_angka(...)' untuk mengubah teks menjadi angka sebelum menghitungnya.";
            } else if (contains(msg, "a nil value")) {
                category = "[Kesalahan Runtime]";
                if (contains(msg, "local '")) {
                    std::string varName = extractBetween(msg, "local '", "'");
                    explanation = "Operasi matematika gagal karena variabel lokal '" + varName + "' bernilai 'nihil' (kosong).";
                } else if (contains(msg, "global '")) {
                    std::string varName = extractBetween(msg, "global '", "'");
                    explanation = "Operasi matematika gagal karena variabel global '" + varName + "' bernilai 'nihil' (tidak ditemukan).";
                } else if (contains(msg, "field '")) {
                    std::string varName = extractBetween(msg, "field '", "'");
                    explanation = "Operasi matematika gagal karena properti '" + varName + "' bernilai 'nihil' (kosong).";
                } else {
                    explanation = "Operasi perhitungan matematika gagal karena salah satu nilainya bernilai 'nihil' (kosong).";
                }
                tips = "Operasi hitung matematika (+, -, *, /, //, %, ^) hanya bisa dilakukan pada angka, bukan nilai kosong (nihil).";
            } else {
                category = "[Kesalahan Tipe Data]";
                std::string typeStr = extractBetween(msg, "on a ", " value");
                if (typeStr.empty()) typeStr = extractBetween(msg, "on ", " value");
                explanation = "Operasi matematika tidak dapat dilakukan pada tipe data " + (typeStr.empty() ? "tersebut" : translateTypeName(typeStr)) + ".";
                tips = "Operasi matematika hanya dapat dilakukan pada tipe data angka.";
            }
        }

        // 5. Runtime: Penggabungan String (..)
        else if (contains(msg, "attempt to concatenate")) {
            if (contains(msg, "a nil value")) {
                category = "[Kesalahan Runtime]";
                if (contains(msg, "local '")) {
                    std::string varName = extractBetween(msg, "local '", "'");
                    explanation = "Gagal menggabungkan teks (operator '..') karena variabel lokal '" + varName + "' bernilai 'nihil' (kosong).";
                } else if (contains(msg, "global '")) {
                    std::string varName = extractBetween(msg, "global '", "'");
                    explanation = "Gagal menggabungkan teks (operator '..') karena variabel global '" + varName + "' bernilai 'nihil' (kosong).";
                } else {
                    explanation = "Gagal menggabungkan teks (operator '..') karena nilai bernilai 'nihil' (kosong).";
                }
                tips = "Operator penggabungan teks (..) memerlukan teks atau angka. Gunakan 'ke_teks(...)' atau pastikan variabel tidak kosong.";
            } else {
                category = "[Kesalahan Tipe Data]";
                std::string typeStr = extractBetween(msg, "a ", " value");
                explanation = "Gagal menggabungkan teks dengan tipe data " + translateTypeName(typeStr) + ".";
                tips = "Ubah nilai tersebut menjadi teks menggunakan fungsi 'ke_teks(...)' terlebih dahulu sebelum digabungkan dengan '..'.";
            }
        }

        // 6. Runtime: Perbandingan Nilai (<, <=, >, >=)
        else if (contains(msg, "attempt to compare")) {
            category = "[Kesalahan Tipe Data]";
            if (contains(msg, " with ")) {
                std::string typeA = extractBetween(msg, "attempt to compare ", " with ");
                std::string typeB = extractAfter(msg, " with ");
                explanation = "Tidak dapat membandingkan tipe data yang berbeda (" + translateTypeName(typeA) + " dengan " + translateTypeName(typeB) + ") menggunakan operator perbandingan (<, <=, >, >=).";
                tips = "Kedua nilai yang dibandingkan harus memiliki tipe data yang sama (misal sesama angka atau sesama teks).";
            } else if (contains(msg, "two ") && contains(msg, " values")) {
                std::string typeA = extractBetween(msg, "two ", " values");
                explanation = "Tipe data " + translateTypeName(typeA) + " tidak mendukung operasi perbandingan (<, <=, >, >=).";
                tips = "Hanya angka dan string teks yang dapat dibandingkan dengan operator lebih besar atau lebih kecil.";
            } else {
                explanation = "Operasi perbandingan nilai tidak valid pada tipe data ini.";
            }
        }

        // 7. Runtime: Indeks Tabel
        else if (contains(msg, "table index is nil")) {
            category = "[Kesalahan Runtime]";
            explanation = "Kunci atau indeks tabel tidak boleh bernilai 'nihil' (kosong).";
            tips = "Saat mengisi atau membaca data tabel (contoh: tabel[kunci] = nilai), variabel 'kunci' tidak boleh kosong/nihil.";
        }
        else if (contains(msg, "table index is NaN")) {
            category = "[Kesalahan Runtime]";
            explanation = "Kunci atau indeks tabel bernilai NaN (Bukan Angka).";
            tips = "Nilai indeks tabel harus berupa angka terdefinisi atau teks string.";
        }

        // 8. Memory / Recursion: Stack Overflow
        else if (contains(msg, "stack overflow")) {
            category = "[Kesalahan Memori/Rekursi]";
            explanation = "Batas kapasitas penumpukan fungsi terlampaui (Stack Overflow).";
            tips = "Terjadi pemanggilan fungsi berulang-ulang tanpa henti (rekursi tak terbatas). Pastikan fungsi Anda memiliki kondisi henti (base case) yang valid.";
        }

        // 9. Bad Argument
        else if (contains(msg, "bad argument #")) {
            category = "[Kesalahan Argumen]";
            std::string argNum = extractBetween(msg, "bad argument #", " ");
            std::string fnName = extractBetween(msg, "to '", "'");
            if (contains(msg, "expected, got")) {
                std::string expectedType = extractBetween(msg, "(", " expected");
                std::string gotType = extractBetween(msg, "got ", ")");
                explanation = "Argumen ke-" + argNum + " pada fungsi '" + fnName + "' tidak valid: Diharapkan bertipe " + translateTypeName(expectedType) + ", namun diberikan " + translateTypeName(gotType) + ".";
                tips = "Periksa data yang Anda kirimkan saat memanggil fungsi '" + fnName + "'. Pastikan tipenya sesuai.";
            } else {
                std::string detail = extractBetween(msg, "(", ")");
                if (!detail.empty()) {
                    explanation = "Argumen ke-" + argNum + " pada fungsi '" + fnName + "' tidak valid: " + detail + ".";
                } else {
                    explanation = "Argumen ke-" + argNum + " pada fungsi '" + fnName + "' tidak valid.";
                }
                tips = "Periksa kembali parameter yang dikirim ke fungsi '" + fnName + "'.";
            }
        }

        // 10. Modul Tidak Ditemukan
        else if (contains(msg, "not found:") && contains(msg, "module '")) {
            category = "[Kesalahan Modul]";
            std::string modName = extractBetween(msg, "module '", "'");
            explanation = "Modul '" + modName + "' tidak dapat ditemukan di direktori proyek atau pencarian pustaka.";
            tips = "Pastikan berkas '" + modName + ".luai' atau '" + modName + ".lua' berada di direktori yang sama dengan skrip Anda.";
        }
        else if (contains(msg, "loop or previous error loading module '")) {
            category = "[Kesalahan Modul]";
            std::string modName = extractBetween(msg, "loading module '", "'");
            explanation = "Terjadi dependensi melingkar (circular import) atau kesalahan saat memuat modul '" + modName + "'.";
            tips = "Periksa apakah modul '" + modName + "' saling mengimpor dengan modul lain tanpa henti.";
        }

        // 11. Berkas Tidak Dapat Dibuka
        else if (contains(msg, "Tidak dapat membuka file:") || contains(msg, "cannot open file")) {
            category = "[Kesalahan Berkas]";
            std::string fileName = extractAfter(msg, "file:");
            if (fileName.empty()) fileName = extractBetween(msg, "cannot open file '", "'");
            trim(fileName);
            explanation = "Berkas '" + fileName + "' tidak dapat dibuka atau tidak ditemukan.";
            tips = "Pastikan nama berkas dieja dengan benar dan berkas tersebut memang ada di direktori kerja.";
        }

        // 12. Pesan bawaan Luai / User custom error
        else {
            category = "[Kesalahan Program]";
            explanation = msg;
        }

        // Rakit pesan akhir yang ramah pemula
        std::ostringstream out;
        out << category;
        if (loc.hasLocation) {
            out << " Baris " << loc.line;
            if (!loc.source.empty()) {
                out << " di " << loc.source;
            }
        }
        out << ":\n";
        out << "  -> " << explanation << "\n";
        if (!tips.empty()) {
            out << "  Tips: " << tips << "\n";
        }

        return out.str();
    }

private:
    static ErrorLocation parseLocation(const std::string& raw) {
        ErrorLocation info;
        info.message = raw;

        size_t colon1 = std::string::npos;
        size_t colon2 = std::string::npos;

        // Cari pola :<angka>: untuk menemukan lokasi nomor baris
        for (size_t i = 0; i < raw.size(); ++i) {
            if (raw[i] == ':') {
                size_t j = i + 1;
                while (j < raw.size() && std::isdigit(static_cast<unsigned char>(raw[j]))) {
                    ++j;
                }
                if (j > i + 1 && j < raw.size() && raw[j] == ':') {
                    colon1 = i;
                    colon2 = j;
                    break;
                }
            }
        }

        if (colon1 != std::string::npos && colon2 != std::string::npos) {
            info.source = raw.substr(0, colon1);
            std::string lineStr = raw.substr(colon1 + 1, colon2 - (colon1 + 1));
            try {
                info.line = std::stoi(lineStr);
            } catch (...) {
                info.line = 0;
            }
            size_t msgStart = colon2 + 1;
            while (msgStart < raw.size() && raw[msgStart] == ' ') {
                ++msgStart;
            }
            info.message = raw.substr(msgStart);
            info.hasLocation = true;

            // Rapikan representasi nama sumber
            if (!info.source.empty() && info.source.front() == '@') {
                info.source = info.source.substr(1);
            }
            if (info.source == "[string \"=luai\"]" || info.source == "=luai") {
                info.source = "Sesi Interaktif (REPL)";
            } else if (info.source == "[string \"chunk\"]" || info.source == "chunk") {
                info.source = "Kode";
            } else if (info.source == "baris_perintah" || info.source == "=baris_perintah") {
                info.source = "Baris Perintah ('-e')";
            }
        }

        return info;
    }

    static std::string translateKeyword(const std::string& kw) {
        if (kw == "then") return "maka";
        if (kw == "do") return "lakukan";
        if (kw == "end") return "selesai";
        if (kw == "until") return "sampai";
        if (kw == "function") return "fungsi";
        if (kw == "local") return "lokal";
        if (kw == "return") return "kembalikan";
        if (kw == "if") return "jika";
        if (kw == "else") return "selain_itu";
        if (kw == "elseif") return "atau_jika";
        if (kw == "for") return "untuk";
        if (kw == "while") return "selama";
        if (kw == "repeat") return "ulang";
        if (kw == "break") return "berhenti";
        if (kw == "in") return "dalam";
        if (kw == "nil") return "nihil";
        if (kw == "true") return "benar";
        if (kw == "false") return "salah";
        return kw;
    }

    static std::string translateTypeName(const std::string& type) {
        if (type == "nil") return "nihil (kosong)";
        if (type == "number") return "angka";
        if (type == "string") return "teks (string)";
        if (type == "table") return "tabel";
        if (type == "boolean") return "boolean (benar/salah)";
        if (type == "function") return "fungsi";
        if (type == "userdata") return "data sistem (userdata)";
        if (type == "thread") return "korutin (thread)";
        return type;
    }

    static bool contains(const std::string& str, const std::string& sub) {
        return str.find(sub) != std::string::npos;
    }

    static std::string extractNear(const std::string& msg) {
        size_t pos = msg.rfind("near '");
        if (pos != std::string::npos) {
            size_t start = pos + 6;
            size_t end = msg.find("'", start);
            if (end != std::string::npos) {
                return msg.substr(start, end - start);
            }
            return msg.substr(start);
        }
        return "";
    }

    static std::string extractBetween(const std::string& str, const std::string& startDelim, const std::string& endDelim) {
        size_t start = str.find(startDelim);
        if (start == std::string::npos) return "";
        start += startDelim.size();
        size_t end = str.find(endDelim, start);
        if (end == std::string::npos) return "";
        return str.substr(start, end - start);
    }

    static std::string extractAfter(const std::string& str, const std::string& delim) {
        size_t pos = str.find(delim);
        if (pos == std::string::npos) return "";
        return str.substr(pos + delim.size());
    }

    static void trim(std::string& s) {
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) {
            s.erase(s.begin());
        }
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
            s.pop_back();
        }
    }
};

} // namespace Luai

#endif // LUAI_ERROR_HANDLER_HPP
