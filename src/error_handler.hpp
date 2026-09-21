#ifndef LUAI_ERROR_HANDLER_HPP
#define LUAI_ERROR_HANDLER_HPP

#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <fstream>

namespace Luai {

struct ErrorLocation {
    std::string source;
    int line = 0;
    std::string message;
    std::string targetToken;
    bool hasLocation = false;
};

class ErrorHandler {
public:
    static std::string format(const std::string& raw, const std::string& sourceCode = "", const std::string& fallbackPath = "") {
        if (raw.empty()) {
            return raw;
        }

        // Jangan format ulang jika pesan error sudah berformat Luai
        if (raw.rfind("[Kesalahan", 0) == 0) {
            return raw;
        }

        ErrorLocation loc = parseLocation(raw);
        std::string msg = loc.message;
        trim(msg);

        // Ambil isi kode sumber jika belum disediakan
        std::string code = sourceCode;
        if (code.empty()) {
            std::string tryPath = !loc.source.empty() ? loc.source : fallbackPath;
            if (!tryPath.empty() && tryPath.front() != '=' && tryPath.find("Baris Perintah") == std::string::npos && tryPath.find("Sesi Interaktif") == std::string::npos) {
                std::ifstream f(tryPath);
                if (f.is_open()) {
                    code.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
                }
            }
        }

        std::string category = "[Kesalahan Runtime]";
        std::string explanation;
        std::string recommendation;
        std::string tips;
        std::string targetToken;

        // 1. Parsing Kesalahan Sintaks Kompilasi
        if (contains(msg, "'then' expected")) {
            category = "[Kesalahan Sintaks]";
            targetToken = extractNear(msg);
            if (!targetToken.empty() && targetToken != "<eof>") {
                explanation = "Diharapkan kata kunci 'maka' setelah kondisi percabangan 'jika', namun ditemukan '" + targetToken + "'.";
                if (levenshteinDistance(targetToken, "maka") <= 2 && targetToken != "maka") {
                    recommendation = "Apakah maksud Anda kata kunci 'maka' (bukan '" + targetToken + "')?";
                }
            } else {
                explanation = "Diharapkan kata kunci 'maka' setelah kondisi percabangan 'jika'.";
            }
            tips = "Format percabangan di Luai adalah: jika <kondisi> maka ... selesai";
        }
        else if (contains(msg, "'do' expected")) {
            category = "[Kesalahan Sintaks]";
            targetToken = extractNear(msg);
            if (!targetToken.empty() && targetToken != "<eof>") {
                explanation = "Diharapkan kata kunci 'lakukan' pada blok perulangan, namun ditemukan '" + targetToken + "'.";
                if (levenshteinDistance(targetToken, "lakukan") <= 2 && targetToken != "lakukan") {
                    recommendation = "Apakah maksud Anda kata kunci 'lakukan' (bukan '" + targetToken + "')?";
                }
            } else {
                explanation = "Diharapkan kata kunci 'lakukan' pada blok perulangan.";
            }
            tips = "Format perulangan di Luai adalah: untuk ... lakukan ... selesai atau selama <kondisi> lakukan ... selesai";
        }
        else if (contains(msg, "'end' expected (to close '")) {
            category = "[Kesalahan Sintaks]";
            std::string blockName = extractBetween(msg, "'end' expected (to close '", "'");
            std::string atLine = extractBetween(msg, "at line ", ")");
            targetToken = extractNear(msg);
            std::string blockId = translateKeyword(blockName);
            explanation = "Blok '" + blockId + "' (yang dibuka pada baris " + (atLine.empty() ? "?" : atLine) + ") belum ditutup dengan kata kunci 'selesai'.";
            if (!targetToken.empty() && targetToken != "<eof>") {
                explanation += " Ditemukan '" + targetToken + "' sebelum blok selesai.";
            }
            tips = "Tambahkan kata kunci 'selesai' untuk menutup blok '" + blockId + "' tersebut.";
        }
        else if (contains(msg, "'end' expected")) {
            category = "[Kesalahan Sintaks]";
            targetToken = extractNear(msg);
            if (targetToken == "<eof>") {
                explanation = "Blok kode belum ditutup. Diharapkan kata kunci penutup 'selesai' sebelum akhir berkas/kode.";
            } else if (!targetToken.empty()) {
                explanation = "Diharapkan kata kunci penutup 'selesai' di dekat '" + targetToken + "'.";
                if (levenshteinDistance(targetToken, "selesai") <= 2 && targetToken != "selesai") {
                    recommendation = "Apakah maksud Anda kata kunci 'selesai' (bukan '" + targetToken + "')?";
                }
            } else {
                explanation = "Blok kode belum ditutup. Diharapkan kata kunci 'selesai'.";
            }
            tips = "Pastikan semua blok percabangan ('jika'), perulangan ('untuk', 'selama'), dan 'fungsi' sudah ditutup dengan kata 'selesai'.";
        }
        else if (contains(msg, "'until' expected")) {
            category = "[Kesalahan Sintaks]";
            targetToken = extractNear(msg);
            explanation = "Diharapkan kata kunci 'sampai' untuk mengakhiri perulangan 'ulang'.";
            if (!targetToken.empty() && targetToken != "<eof>") {
                explanation += " Ditemukan '" + targetToken + "'.";
                if (levenshteinDistance(targetToken, "sampai") <= 2 && targetToken != "sampai") {
                    recommendation = "Apakah maksud Anda kata kunci 'sampai' (bukan '" + targetToken + "')?";
                }
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
            targetToken = extractNear(msg);
            explanation = "Teks string belum ditutup.";
            if (!targetToken.empty()) {
                explanation += " Terjadi di dekat '" + targetToken + "'.";
            }
            tips = "Pastikan tanda kutip pembuka (baik ' maupun \") memiliki pasangan tanda kutip penutup yang cocok pada baris yang sama.";
        }
        else if (contains(msg, "unexpected symbol near")) {
            category = "[Kesalahan Sintaks]";
            targetToken = extractNear(msg);
            explanation = "Ditemukan simbol atau karakter tak terduga '" + targetToken + "' yang tidak dikenali sintaks Luai.";
            
            // Periksa apakah simbol tak terduga adalah typo dari kata kunci atau fungsi bawaan
            if (!targetToken.empty() && targetToken != "<eof>") {
                std::vector<std::string> candidates = getAllKeywords();
                std::vector<std::string> globals = getAllGlobalFunctions();
                candidates.insert(candidates.end(), globals.begin(), globals.end());
                std::string match = findClosestMatch(targetToken, candidates);
                if (!match.empty()) {
                    recommendation = "Apakah maksud Anda: '" + match + "'?";
                }
            }
            tips = "Periksa apakah ada tanda baca yang salah ketik, tanda kurung yang belum ditutup, atau operator yang kurang.";
        }
        else if (contains(msg, "'=' expected")) {
            category = "[Kesalahan Sintaks]";
            targetToken = extractNear(msg);
            explanation = "Diharapkan tanda sama dengan '=' untuk pemberian nilai (assignment).";
            if (!targetToken.empty() && targetToken != "<eof>") {
                std::vector<std::string> candidates = getAllKeywords();
                std::string match = findClosestMatch(targetToken, candidates);
                if (!match.empty()) {
                    recommendation = "Apakah maksud Anda kata kunci: '" + match + "'?";
                }
            }
            tips = "Contoh penulisan: lokal nama = \"Budi\"";
        }
        else if (contains(msg, "'(' expected")) {
            category = "[Kesalahan Sintaks]";
            targetToken = extractNear(msg);
            explanation = "Diharapkan tanda kurung buka '('.";
            tips = "Periksa kembali sintaks pemanggilan fungsi atau pengelompokan ekspresi.";
        }
        else if (contains(msg, "')' expected")) {
            category = "[Kesalahan Sintaks]";
            targetToken = extractNear(msg);
            explanation = "Diharapkan tanda kurung tutup ')'.";
            tips = "Pastikan setiap tanda kurung buka '(' sudah memiliki pasangan penutup ')'.";
        }
        else if (contains(msg, "']' expected")) {
            category = "[Kesalahan Sintaks]";
            targetToken = extractNear(msg);
            explanation = "Diharapkan tanda kurung siku penutup ']'.";
            tips = "Pastikan pengindeksan tabel tabel[...] ditutup dengan tanda ']'.";
        }
        else if (contains(msg, "'}' expected")) {
            category = "[Kesalahan Sintaks]";
            targetToken = extractNear(msg);
            explanation = "Diharapkan tanda kurung kurawal penutup '}' untuk menutup definisi tabel.";
            tips = "Pastikan definisi tabel { ... } diakhiri dengan tanda '}'.";
        }
        else if (contains(msg, "<name> expected")) {
            category = "[Kesalahan Sintaks]";
            targetToken = extractNear(msg);
            explanation = "Diharapkan nama variabel atau fungsi yang valid.";
            if (!targetToken.empty()) {
                explanation += " Ditemukan '" + targetToken + "'.";
                std::vector<std::string> candidates = getAllKeywords();
                std::string match = findClosestMatch(targetToken, candidates);
                if (!match.empty()) {
                    recommendation = "Apakah maksud Anda: '" + match + "'?";
                }
            }
            tips = "Nama variabel dan fungsi harus diawali dengan huruf atau garis bawah (_), bukan angka atau simbol khusus.";
        }
        else if (contains(msg, "no loop to break")) {
            category = "[Kesalahan Sintaks]";
            explanation = "Perintah 'berhenti' (break) hanya boleh digunakan di dalam blok perulangan ('untuk', 'selama', atau 'ulang').";
            tips = "Hapus perintah 'berhenti' jika kode ini tidak berada di dalam blok perulangan.";
        }

        // 2. Runtime: Indexing nil atau bukan tabel
        else if (contains(msg, "attempt to index local '")) {
            category = "[Kesalahan Runtime]";
            targetToken = extractBetween(msg, "attempt to index local '", "'");
            explanation = "Mencoba mengakses properti/indeks dari variabel lokal '" + targetToken + "' yang bernilai 'nihil' (kosong).";
            
            // Rekomendasi nama variabel lokal terdekat
            std::vector<std::string> candidates = extractUserIdentifiers(code);
            std::string match = findClosestMatch(targetToken, candidates);
            if (!match.empty()) {
                recommendation = "Apakah maksud Anda variabel: '" + match + "'?";
            }
            tips = "Variabel '" + targetToken + "' belum memiliki isi tabel. Berikan nilai tabel terlebih dahulu (contoh: lokal " + targetToken + " = {}) sebelum mengakses isinya.";
        }
        else if (contains(msg, "attempt to index global '")) {
            category = "[Kesalahan Runtime]";
            targetToken = extractBetween(msg, "attempt to index global '", "'");
            explanation = "Mencoba mengakses properti/indeks dari variabel global '" + targetToken + "' yang bernilai 'nihil' (belum dibuat/didefinisikan).";
            
            // Cari rekomendasi dari variabel program, modul, atau fungsi
            std::vector<std::string> candidates = getAllGlobalFunctions();
            std::vector<std::string> userIds = extractUserIdentifiers(code);
            candidates.insert(candidates.end(), userIds.begin(), userIds.end());
            std::string match = findClosestMatch(targetToken, candidates);
            if (!match.empty()) {
                recommendation = "Apakah maksud Anda: '" + match + "'?";
            }
            tips = "Variabel atau modul '" + targetToken + "' tidak ditemukan. Pastikan nama variabel benar atau modul sudah dimuat dengan 'butuh(\"" + targetToken + "\")'.";
        }
        else if (contains(msg, "attempt to index field '")) {
            category = "[Kesalahan Runtime]";
            targetToken = extractBetween(msg, "attempt to index field '", "'");
            explanation = "Mencoba mengakses sub-properti dari kolom/field '" + targetToken + "' yang bernilai 'nihil' (kosong).";
            
            std::vector<std::string> candidates = getAllModuleMembers();
            std::vector<std::string> userIds = extractUserIdentifiers(code);
            candidates.insert(candidates.end(), userIds.begin(), userIds.end());
            std::string match = findClosestMatch(targetToken, candidates);
            if (!match.empty()) {
                recommendation = "Apakah maksud Anda: '" + match + "'?";
            }
            tips = "Tabel tidak memiliki kolom bernama '" + targetToken + "' atau kolom tersebut bernilai kosong.";
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
            targetToken = extractBetween(msg, "attempt to call local '", "'");
            explanation = "Mencoba memanggil variabel lokal '" + targetToken + "' sebagai fungsi, namun bernilai 'nihil' (bukan fungsi).";
            
            std::vector<std::string> candidates = extractUserIdentifiers(code);
            std::string match = findClosestMatch(targetToken, candidates);
            if (!match.empty()) {
                recommendation = "Apakah maksud Anda fungsi: '" + match + "'?";
            }
            tips = "Fungsi lokal '" + targetToken + "' belum dibuat atau belum didefinisikan sebelum baris pemanggilan ini.";
        }
        else if (contains(msg, "attempt to call global '")) {
            category = "[Kesalahan Runtime]";
            targetToken = extractBetween(msg, "attempt to call global '", "'");
            explanation = "Mencoba memanggil fungsi global '" + targetToken + "', namun fungsi tersebut tidak ditemukan (bernilai 'nihil').";
            
            // Cari rekomendasi typo termirip (seperti cwtak -> cetak)
            std::vector<std::string> candidates = getAllGlobalFunctions();
            std::vector<std::string> userIds = extractUserIdentifiers(code);
            candidates.insert(candidates.end(), userIds.begin(), userIds.end());
            std::string match = findClosestMatch(targetToken, candidates);
            if (!match.empty()) {
                recommendation = "Apakah maksud Anda: '" + match + "'?";
            }
            tips = "Periksa penulisan nama fungsi '" + targetToken + "'. Pastikan ejaan huruf besar/kecil sesuai dan fungsi sudah dibuat sebelum dipanggil.";
        }
        else if (contains(msg, "attempt to call field '")) {
            category = "[Kesalahan Runtime]";
            targetToken = extractBetween(msg, "attempt to call field '", "'");
            explanation = "Mencoba memanggil fungsi '" + targetToken + "' pada modul/tabel, namun fungsi tersebut tidak ada (bernilai 'nihil').";
            
            std::vector<std::string> candidates = getAllModuleMembers();
            std::string match = findClosestMatch(targetToken, candidates);
            if (!match.empty()) {
                recommendation = "Apakah maksud Anda: '" + match + "'?";
            }
            tips = "Modul atau tabel tidak memiliki fungsi bernama '" + targetToken + "'. Periksa kembali daftar fungsi pada modul tersebut.";
        }
        else if (contains(msg, "attempt to call method '")) {
            category = "[Kesalahan Runtime]";
            targetToken = extractBetween(msg, "attempt to call method '", "'");
            explanation = "Mencoba memanggil metode '" + targetToken + "' pada objek, namun metode tersebut tidak ditemukan (bernilai 'nihil').";
            
            std::vector<std::string> candidates = getFileHandleMethods();
            std::string match = findClosestMatch(targetToken, candidates);
            if (!match.empty()) {
                recommendation = "Apakah maksud Anda metode: '" + match + "'?";
            }
            tips = "Periksa penulisan nama metode '" + targetToken + "'. Misalnya pada objek berkas gunakan metode seperti 'f:tulis(...)', 'f:baca(...)', dll.";
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
                    targetToken = extractBetween(msg, "local '", "'");
                    explanation = "Operasi matematika gagal karena variabel lokal '" + targetToken + "' bernilai 'nihil' (kosong).";
                } else if (contains(msg, "global '")) {
                    targetToken = extractBetween(msg, "global '", "'");
                    explanation = "Operasi matematika gagal karena variabel global '" + targetToken + "' bernilai 'nihil' (tidak ditemukan).";
                    std::vector<std::string> candidates = extractUserIdentifiers(code);
                    std::string match = findClosestMatch(targetToken, candidates);
                    if (!match.empty()) {
                        recommendation = "Apakah maksud Anda variabel: '" + match + "'?";
                    }
                } else if (contains(msg, "field '")) {
                    targetToken = extractBetween(msg, "field '", "'");
                    explanation = "Operasi matematika gagal karena properti '" + targetToken + "' bernilai 'nihil' (kosong).";
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
                    targetToken = extractBetween(msg, "local '", "'");
                    explanation = "Gagal menggabungkan teks (operator '..') karena variabel lokal '" + targetToken + "' bernilai 'nihil' (kosong).";
                } else if (contains(msg, "global '")) {
                    targetToken = extractBetween(msg, "global '", "'");
                    explanation = "Gagal menggabungkan teks (operator '..') karena variabel global '" + targetToken + "' bernilai 'nihil' (kosong).";
                    std::vector<std::string> candidates = extractUserIdentifiers(code);
                    std::string match = findClosestMatch(targetToken, candidates);
                    if (!match.empty()) {
                        recommendation = "Apakah maksud Anda variabel: '" + match + "'?";
                    }
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
            targetToken = fnName;
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
            targetToken = extractBetween(msg, "module '", "'");
            explanation = "Modul '" + targetToken + "' tidak dapat ditemukan di direktori proyek atau pustaka.";
            
            std::vector<std::string> standardMods = {"matematika", "tabel", "teks", "sistem", "berkas", "io", "korutin", "json", "https", "http"};
            std::string match = findClosestMatch(targetToken, standardMods);
            if (!match.empty()) {
                recommendation = "Apakah maksud Anda modul bawaan: '" + match + "'?";
            }
            tips = "Pastikan berkas '" + targetToken + ".luai' atau '" + targetToken + ".lua' berada di direktori yang sama dengan skrip Anda.";
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

        // Rakit pesan akhir yang ramah pemula dengan konteks kode sumber
        std::ostringstream out;
        out << category;
        if (loc.hasLocation) {
            out << " Baris " << loc.line;
            if (!loc.source.empty()) {
                out << " di " << loc.source;
            }
        }
        out << ":\n";

        // Tampilkan cuplikan baris kode yang bermasalah (seperti Python / Clang)
        if (loc.hasLocation && loc.line > 0 && !code.empty()) {
            std::string snippet = formatSnippet(code, loc.line, targetToken);
            if (!snippet.empty()) {
                out << snippet << "\n";
            }
        }

        out << "  -> " << explanation << "\n";
        if (!recommendation.empty()) {
            out << "  -> " << recommendation << "\n";
        }
        if (!tips.empty()) {
            out << "  Tips: " << tips << "\n";
        }

        return out.str();
    }

    // Algoritma jarak Levenshtein (case-insensitive) untuk mendeteksi typo
    static size_t levenshteinDistance(const std::string& s1, const std::string& s2) {
        const size_t m = s1.size();
        const size_t n = s2.size();
        if (m == 0) return n;
        if (n == 0) return m;

        std::vector<size_t> prev(n + 1);
        std::vector<size_t> curr(n + 1);

        for (size_t j = 0; j <= n; ++j) {
            prev[j] = j;
        }

        for (size_t i = 1; i <= m; ++i) {
            curr[0] = i;
            char c1 = std::tolower(static_cast<unsigned char>(s1[i - 1]));
            for (size_t j = 1; j <= n; ++j) {
                char c2 = std::tolower(static_cast<unsigned char>(s2[j - 1]));
                size_t cost = (c1 == c2) ? 0 : 1;
                curr[j] = std::min({
                    prev[j] + 1,        // deletion
                    curr[j - 1] + 1,    // insertion
                    prev[j - 1] + cost  // substitution
                });
            }
            prev = curr;
        }

        return curr[n];
    }

    // Mencari kata terdekat dari daftar kandidat berdasarkan jarak Levenshtein
    static std::string findClosestMatch(const std::string& word, const std::vector<std::string>& candidates) {
        if (word.empty()) return "";
        std::string bestCandidate;
        size_t minDistance = static_cast<size_t>(-1);

        // Batas toleransi jarak edit berdasarkan panjang kata
        size_t maxAllowed = (word.size() <= 4) ? 1 : (word.size() <= 8 ? 2 : 3);

        for (const auto& cand : candidates) {
            if (cand.empty() || cand == word) continue;

            size_t dist = levenshteinDistance(word, cand);
            if (dist <= maxAllowed && dist < minDistance) {
                minDistance = dist;
                bestCandidate = cand;
            }
        }

        return bestCandidate;
    }

    // Daftar kata kunci resmi Luai
    static std::vector<std::string> getAllKeywords() {
        return {
            "lokal", "fungsi", "kembalikan", "jika", "maka", "atau_jika", "selain_jika", "selain_itu", "selesai",
            "untuk", "selama", "ulang", "sampai", "dalam", "lakukan", "berhenti", "lanjut",
            "benar", "salah", "nihil", "kosong", "dan", "atau", "bukan", "lompat_ke"
        };
    }

    // Daftar fungsi bawaan global dan modul Luai
    static std::vector<std::string> getAllGlobalFunctions() {
        return {
            "cetak", "masukan", "minta", "tanya", "baca", "tipe", "ke_angka", "ke_teks",
            "pasangan", "i_pasangan", "lepas", "tegaskan", "kesalahan",
            "pcall_aman", "panggil_aman", "xpcall_aman",
            "muat", "muat_file", "eksekusi_file", "butuh", "perlu",
            "koleksi_sampah", "pilih", "set_metatabel", "ambil_metatabel",
            // Nama modul bawaan
            "matematika", "tabel", "teks", "sistem", "berkas", "io", "korutin", "json", "https", "http"
        };
    }

    // Daftar semua anggota fungsi modul bawaan
    static std::vector<std::string> getAllModuleMembers() {
        return {
            // matematika
            "akar", "mutlak", "lantai", "atap", "acak", "benih_acak", "minimum", "maksimum", "derajat", "radian", "pi", "sin", "cos", "tan",
            // tabel
            "sisip", "hapus", "urut", "gabung", "pindah", "bungkus", "lepas",
            // teks
            "panjang", "huruf_besar", "huruf_kecil", "potong", "cari", "ganti", "cocok", "format", "ulang", "balik", "karakter", "byte",
            // sistem
            "waktu", "jam", "tanggal", "keluar", "jalankan", "ambil_env", "ganti_nama",
            // berkas & io
            "buka", "baca", "tulis", "tutup", "siram", "bilas", "baris", "file_sementara", "berkas_masukan", "berkas_keluaran",
            // json
            "kodekan", "uraikan", "uraikan_aman", "encode", "decode",
            // https
            "ambil", "kirim", "taruh", "permintaan"
        };
    }

    // Daftar metode file handle
    static std::vector<std::string> getFileHandleMethods() {
        return {
            "tulis", "baca", "tutup", "siram", "bilas", "baris", "geser", "posisi", "atur_buffer"
        };
    }

    // Ekstraksi identifier (nama variabel/fungsi) yang didefinisikan pengguna di skrip
    static std::vector<std::string> extractUserIdentifiers(const std::string& code) {
        std::vector<std::string> idents;
        size_t i = 0;
        while (i < code.size()) {
            if (std::isalpha(static_cast<unsigned char>(code[i])) || code[i] == '_') {
                size_t start = i;
                while (i < code.size() && (std::isalnum(static_cast<unsigned char>(code[i])) || code[i] == '_')) {
                    ++i;
                }
                std::string id = code.substr(start, i - start);
                if (id.size() >= 2) {
                    idents.push_back(id);
                }
            } else {
                ++i;
            }
        }
        return idents;
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

    // Format cuplikan baris kode yang menyebabkan kesalahan beserta pointer ^
    static std::string formatSnippet(const std::string& sourceCode, int targetLine, const std::string& token) {
        if (targetLine <= 0 || sourceCode.empty()) {
            return "";
        }

        std::istringstream stream(sourceCode);
        std::string line;
        int currentLine = 1;
        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (currentLine == targetLine) {
                break;
            }
            ++currentLine;
        }

        if (currentLine != targetLine) {
            return "";
        }

        // Ganti karakter tab dengan 4 spasi agar visual pointer ^ sejajar
        std::string cleanLine;
        for (char c : line) {
            if (c == '\t') {
                cleanLine += "    ";
            } else {
                cleanLine += c;
            }
        }

        // Tentukan posisi kolom token untuk pointer ^
        size_t col = std::string::npos;
        size_t tokenLen = 1;

        if (!token.empty() && token != "<eof>") {
            col = cleanLine.find(token);
            tokenLen = token.size();
        }

        // Jika token tidak ditemukan, tunjuk teks pertama yang bukan spasi
        if (col == std::string::npos) {
            size_t firstNonSpace = cleanLine.find_first_not_of(" \t");
            if (firstNonSpace != std::string::npos) {
                col = firstNonSpace;
                size_t lastNonSpace = cleanLine.find_last_not_of(" \t");
                tokenLen = (lastNonSpace >= firstNonSpace) ? std::min<size_t>(lastNonSpace - firstNonSpace + 1, 20) : 1;
            } else {
                col = 0;
                tokenLen = 1;
            }
        }

        std::string lineNumStr = std::to_string(targetLine);
        size_t padLen = (lineNumStr.size() < 4) ? 4 - lineNumStr.size() : 1;
        std::string pad(padLen, ' ');

        std::ostringstream snippet;
        snippet << pad << lineNumStr << " | " << cleanLine << "\n";
        snippet << std::string(padLen + lineNumStr.size(), ' ') << " | " << std::string(col, ' ') << std::string(tokenLen, '^');
        return snippet.str();
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
