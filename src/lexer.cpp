#include "lexer.hpp"
#include <cctype>

const std::unordered_map<std::string, std::string>& Lexer::getKeywordMap() {
    static const std::unordered_map<std::string, std::string> map = {
        {"dan", "and"},
        {"berhenti", "break"},
        {"lakukan", "do"},
        {"selain_itu", "else"},
        {"atau_jika", "elseif"},
        {"selesai", "end"},
        {"salah", "false"},
        {"untuk", "for"},
        {"fungsi", "function"},
        {"lompat_ke", "goto"},
        {"jika", "if"},
        {"dalam", "in"},
        {"lokal", "local"},
        {"kosong", "nil"},
        {"nihil", "nil"},
        {"bukan", "not"},
        {"atau", "or"},
        {"ulang", "repeat"},
        {"kembalikan", "return"},
        {"maka", "then"},
        {"benar", "true"},
        {"sampai", "until"},
        {"selama", "while"}
    };
    return map;
}

const std::unordered_map<std::string, std::string>& Lexer::getReverseKeywordMap() {
    static const std::unordered_map<std::string, std::string> map = {
        {"and", "dan"},
        {"break", "berhenti"},
        {"do", "lakukan"},
        {"else", "selain_itu"},
        {"elseif", "atau_jika"},
        {"end", "selesai"},
        {"false", "salah"},
        {"for", "untuk"},
        {"function", "fungsi"},
        {"goto", "lompat_ke"},
        {"if", "jika"},
        {"in", "dalam"},
        {"local", "lokal"},
        {"nil", "nihil"},
        {"not", "bukan"},
        {"or", "atau"},
        {"repeat", "ulang"},
        {"return", "kembalikan"},
        {"then", "maka"},
        {"true", "benar"},
        {"until", "sampai"},
        {"while", "selama"}
    };
    return map;
}

bool Lexer::isKeyword(const std::string& word) {
    const auto& map = getKeywordMap();
    return map.find(word) != map.end();
}

bool Lexer::isLuaKeyword(const std::string& word) {
    const auto& map = getReverseKeywordMap();
    return map.find(word) != map.end();
}

const std::unordered_map<std::string, std::string>& Lexer::getLuaiToLuaGlobalMap() {
    static const std::unordered_map<std::string, std::string> map = {
        {"cetak", "print"},
        {"tipe", "type"},
        {"ke_angka", "tonumber"},
        {"ke_teks", "tostring"},
        {"pasangan", "pairs"},
        {"i_pasangan", "ipairs"},
        {"lepas", "table.unpack"},
        {"tegaskan", "assert"},
        {"kesalahan", "error"},
        {"pcall_aman", "pcall"},
        {"panggil_aman", "pcall"},
        {"xpcall_aman", "xpcall"},
        {"set_metatabel", "setmetatable"},
        {"ambil_metatabel", "getmetatable"},
        {"koleksi_sampah", "collectgarbage"},
        {"pilih", "select"},
        {"muat", "load"},
        {"muat_file", "loadfile"},
        {"eksekusi_file", "dofile"},
        {"butuh", "require"},
        {"perlu", "require"},
        {"masukan", "io.read"},
        {"minta", "io.read"},
        {"tanya", "io.read"},
        {"baca", "io.read"}
    };
    return map;
}

const std::unordered_map<std::string, std::string>& Lexer::getLuaToLuaiGlobalMap() {
    static const std::unordered_map<std::string, std::string> map = {
        {"print", "cetak"},
        {"type", "tipe"},
        {"tonumber", "ke_angka"},
        {"tostring", "ke_teks"},
        {"pairs", "pasangan"},
        {"ipairs", "i_pasangan"},
        {"unpack", "lepas"},
        {"assert", "tegaskan"},
        {"error", "kesalahan"},
        {"pcall", "pcall_aman"},
        {"xpcall", "xpcall_aman"},
        {"setmetatable", "set_metatabel"},
        {"getmetatable", "ambil_metatabel"},
        {"collectgarbage", "koleksi_sampah"},
        {"select", "pilih"},
        {"load", "muat"},
        {"loadfile", "muat_file"},
        {"dofile", "eksekusi_file"},
        {"require", "butuh"}
    };
    return map;
}

const std::unordered_map<std::string, std::string>& Lexer::getLuaiToLuaModuleMap() {
    static const std::unordered_map<std::string, std::string> map = {
        {"matematika", "math"},
        {"tabel", "table"},
        {"teks", "string"},
        {"sistem", "os"},
        {"korutin", "coroutine"},
        {"io", "io"},
        {"json", "json"},
        {"https", "https"},
        {"http", "http"}
    };
    return map;
}

const std::unordered_map<std::string, std::string>& Lexer::getLuaToLuaiModuleMap() {
    static const std::unordered_map<std::string, std::string> map = {
        {"math", "matematika"},
        {"table", "tabel"},
        {"string", "teks"},
        {"os", "sistem"},
        {"coroutine", "korutin"},
        {"io", "io"},
        {"json", "json"},
        {"https", "https"},
        {"http", "http"}
    };
    return map;
}

const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& Lexer::getLuaiToLuaMemberMap() {
    static const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> map = {
        {"math", {
            {"akar", "sqrt"},
            {"mutlak", "abs"},
            {"lantai", "floor"},
            {"atap", "ceil"},
            {"acak", "random"},
            {"benih_acak", "randomseed"},
            {"minimum", "min"},
            {"maksimum", "max"},
            {"derajat", "deg"},
            {"radian", "rad"},
            {"sinus", "sin"},
            {"kosinus", "cos"},
            {"tangen", "tan"},
            {"pi", "pi"},
            {"tak_hingga", "huge"}
        }},
        {"table", {
            {"sisip", "insert"},
            {"hapus", "remove"},
            {"urut", "sort"},
            {"gabung", "concat"},
            {"lepas", "unpack"},
            {"pindah", "move"},
            {"bungkus", "pack"}
        }},
        {"string", {
            {"panjang", "len"},
            {"huruf_besar", "upper"},
            {"huruf_kecil", "lower"},
            {"potong", "sub"},
            {"cari", "find"},
            {"ganti", "gsub"},
            {"cocok", "match"},
            {"format", "format"},
            {"ulang", "rep"},
            {"balik", "reverse"},
            {"karakter", "char"},
            {"byte", "byte"}
        }},
        {"os", {
            {"waktu", "time"},
            {"jam", "clock"},
            {"tanggal", "date"},
            {"keluar", "exit"},
            {"jalankan", "execute"},
            {"ambil_env", "getenv"},
            {"hapus", "remove"},
            {"ganti_nama", "rename"},
            {"set_lokal", "setlocale"},
            {"file_sementara", "tmpname"}
        }},
        {"coroutine", {
            {"buat", "create"},
            {"lanjutkan", "resume"},
            {"hasilkan", "yield"},
            {"status", "status"},
            {"berjalan", "running"},
            {"bungkus", "wrap"}
        }},
        {"io", {
            {"tulis", "write"},
            {"buka", "open"},
            {"tutup", "close"},
            {"siram", "flush"},
            {"baris", "lines"},
            {"tipe", "type"},
            {"masukan", "read"},
            {"minta", "read"},
            {"tanya", "read"},
            {"baca", "read"},
            {"berkas_masukan", "input"},
            {"berkas_keluaran", "output"}
        }},
        {"json", {
            {"kodekan", "encode"},
            {"tulis", "encode"},
            {"ke_json", "encode"},
            {"uraikan", "decode"},
            {"baca", "decode"},
            {"dari_json", "decode"},
            {"uraikan_aman", "decode_safe"},
            {"nihil", "null"}
        }},
        {"https", {
            {"ambil", "get"},
            {"kirim", "post"},
            {"taruh", "put"},
            {"hapus", "delete"},
            {"permintaan", "request"}
        }},
        {"http", {
            {"ambil", "get"},
            {"kirim", "post"},
            {"taruh", "put"},
            {"hapus", "delete"},
            {"permintaan", "request"}
        }}
    };
    return map;
}

const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& Lexer::getLuaToLuaiMemberMap() {
    static const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> map = {
        {"matematika", {
            {"sqrt", "akar"},
            {"abs", "mutlak"},
            {"floor", "lantai"},
            {"ceil", "atap"},
            {"random", "acak"},
            {"randomseed", "benih_acak"},
            {"min", "minimum"},
            {"max", "maksimum"},
            {"deg", "derajat"},
            {"rad", "radian"},
            {"sin", "sinus"},
            {"cos", "kosinus"},
            {"tan", "tangen"},
            {"pi", "pi"},
            {"huge", "tak_hingga"}
        }},
        {"tabel", {
            {"insert", "sisip"},
            {"remove", "hapus"},
            {"sort", "urut"},
            {"concat", "gabung"},
            {"unpack", "lepas"},
            {"move", "pindah"},
            {"pack", "bungkus"}
        }},
        {"teks", {
            {"len", "panjang"},
            {"upper", "huruf_besar"},
            {"lower", "huruf_kecil"},
            {"sub", "potong"},
            {"find", "cari"},
            {"gsub", "ganti"},
            {"match", "cocok"},
            {"format", "format"},
            {"rep", "ulang"},
            {"reverse", "balik"},
            {"char", "karakter"},
            {"byte", "byte"}
        }},
        {"sistem", {
            {"time", "waktu"},
            {"clock", "jam"},
            {"date", "tanggal"},
            {"exit", "keluar"},
            {"execute", "jalankan"},
            {"getenv", "ambil_env"},
            {"remove", "hapus"},
            {"rename", "ganti_nama"},
            {"setlocale", "set_lokal"},
            {"tmpname", "file_sementara"}
        }},
        {"korutin", {
            {"create", "buat"},
            {"resume", "lanjutkan"},
            {"yield", "hasilkan"},
            {"status", "status"},
            {"running", "berjalan"},
            {"wrap", "bungkus"}
        }},
        {"io", {
            {"write", "tulis"},
            {"open", "buka"},
            {"close", "tutup"},
            {"flush", "siram"},
            {"lines", "baris"},
            {"type", "tipe"},
            {"read", "masukan"},
            {"input", "berkas_masukan"},
            {"output", "berkas_keluaran"}
        }},
        {"json", {
            {"encode", "kodekan"},
            {"decode", "uraikan"},
            {"decode_safe", "uraikan_aman"},
            {"null", "nihil"}
        }},
        {"https", {
            {"get", "ambil"},
            {"post", "kirim"},
            {"put", "taruh"},
            {"delete", "hapus"},
            {"request", "permintaan"}
        }},
        {"http", {
            {"get", "ambil"},
            {"post", "kirim"},
            {"put", "taruh"},
            {"delete", "hapus"},
            {"request", "permintaan"}
        }}
    };
    return map;
}

const std::unordered_map<std::string, std::string>& Lexer::getLuaiToLuaMethodMap() {
    static const std::unordered_map<std::string, std::string> map = {
        // File / Berkas handle methods
        {"tulis", "write"},
        {"baca", "read"},
        {"tutup", "close"},
        {"siram", "flush"},
        {"bilas", "flush"},
        {"baris", "lines"},
        {"geser", "seek"},
        {"posisi", "seek"},
        {"cari_posisi", "seek"},
        {"atur_buffer", "setvbuf"},
        {"set_buffer", "setvbuf"},
        {"setel_buffer", "setvbuf"},
        // String methods
        {"panjang", "len"},
        {"huruf_besar", "upper"},
        {"huruf_kecil", "lower"},
        {"potong", "sub"},
        {"cari", "find"},
        {"ganti", "gsub"},
        {"cocok", "match"},
        {"format", "format"},
        {"ulang", "rep"},
        {"balik", "reverse"},
        {"karakter", "char"},
        {"byte", "byte"}
    };
    return map;
}

const std::unordered_map<std::string, std::string>& Lexer::getLuaToLuaiMethodMap() {
    static const std::unordered_map<std::string, std::string> map = {
        // File / Berkas handle methods
        {"write", "tulis"},
        {"read", "baca"},
        {"close", "tutup"},
        {"flush", "siram"},
        {"lines", "baris"},
        {"seek", "geser"},
        {"setvbuf", "atur_buffer"},
        // String methods
        {"len", "panjang"},
        {"upper", "huruf_besar"},
        {"lower", "huruf_kecil"},
        {"sub", "potong"},
        {"find", "cari"},
        {"gsub", "ganti"},
        {"match", "cocok"},
        {"format", "format"},
        {"rep", "ulang"},
        {"reverse", "balik"},
        {"char", "karakter"},
        {"byte", "byte"}
    };
    return map;
}


// ---------------------------------------------------------------------------
// Transpile untuk eksekusi runtime internal
// ---------------------------------------------------------------------------
std::string Lexer::transpile(const std::string& source) {
    std::string result;
    size_t n = source.size();
    size_t i = 0;

    if (n >= 3 && static_cast<unsigned char>(source[0]) == 0xEF &&
        static_cast<unsigned char>(source[1]) == 0xBB &&
        static_cast<unsigned char>(source[2]) == 0xBF) {
        i = 3;
    }

    while (i < n) {
        if (i == 0 && source[i] == '#') {
            while (i < n && source[i] != '\n') {
                result += source[i];
                i++;
            }
            continue;
        }

        if (i + 1 < n && source[i] == '-' && source[i + 1] == '-') {
            size_t j = i + 2;
            if (j < n && source[j] == '[') {
                size_t k = j + 1;
                size_t eq_count = 0;
                while (k < n && source[k] == '=') {
                    eq_count++;
                    k++;
                }
                if (k < n && source[k] == '[') {
                    std::string close_tag = "]" + std::string(eq_count, '=') + "]";
                    size_t close_pos = source.find(close_tag, k + 1);
                    if (close_pos != std::string::npos) {
                        size_t end_pos = close_pos + close_tag.size();
                        result += source.substr(i, end_pos - i);
                        i = end_pos;
                    } else {
                        result += source.substr(i);
                        i = n;
                    }
                    continue;
                }
            }
            while (i < n && source[i] != '\n') {
                result += source[i];
                i++;
            }
            continue;
        }

        if (source[i] == '"' || source[i] == '\'') {
            char quote = source[i];
            result += quote;
            i++;
            while (i < n) {
                char sc = source[i];
                if (sc == '\\') {
                    result += sc;
                    i++;
                    if (i < n) {
                        result += source[i];
                        i++;
                    }
                } else if (sc == quote) {
                    result += sc;
                    i++;
                    break;
                } else {
                    result += sc;
                    i++;
                }
            }
            continue;
        }

        if (source[i] == '[') {
            size_t j = i + 1;
            size_t eq_count = 0;
            while (j < n && source[j] == '=') {
                eq_count++;
                j++;
            }
            if (j < n && source[j] == '[') {
                std::string close_tag = "]" + std::string(eq_count, '=') + "]";
                size_t close_pos = source.find(close_tag, j + 1);
                if (close_pos != std::string::npos) {
                    size_t end_pos = close_pos + close_tag.size();
                    result += source.substr(i, end_pos - i);
                    i = end_pos;
                } else {
                    result += source.substr(i);
                    i = n;
                }
                continue;
            }
        }

        // Check for .. (concat) and ... (vararg)
        if (source[i] == '.' && i + 1 < n && source[i + 1] == '.') {
            if (i + 2 < n && source[i + 2] == '.') {
                result += "...";
                i += 3;
                continue;
            }
            if (!result.empty() && std::isdigit(static_cast<unsigned char>(result.back()))) {
                result += ' ';
            }
            result += "..";
            i += 2;
            if (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                result += ' ';
            }
            continue;
        }

        // Check for // (floor division) operator
        if (source[i] == '/' && i + 1 < n && source[i + 1] == '/') {
            size_t p = result.find_last_not_of(" \t\r\n");
            if (p != std::string::npos) {
                size_t lhs_start = p;
                if (result[p] == ')') {
                    int depth = 1;
                    while (lhs_start > 0 && depth > 0) {
                        lhs_start--;
                        if (result[lhs_start] == ')') depth++;
                        else if (result[lhs_start] == '(') depth--;
                    }
                    while (lhs_start > 0 && (std::isalnum(static_cast<unsigned char>(result[lhs_start - 1])) ||
                           result[lhs_start - 1] == '_' || result[lhs_start - 1] == '.' || result[lhs_start - 1] == ':')) {
                        lhs_start--;
                    }
                } else if (result[p] == ']') {
                    int depth = 1;
                    while (lhs_start > 0 && depth > 0) {
                        lhs_start--;
                        if (result[lhs_start] == ']') depth++;
                        else if (result[lhs_start] == '[') depth--;
                    }
                    while (lhs_start > 0 && (std::isalnum(static_cast<unsigned char>(result[lhs_start - 1])) ||
                           result[lhs_start - 1] == '_' || result[lhs_start - 1] == '.' || result[lhs_start - 1] == ':')) {
                        lhs_start--;
                    }
                } else {
                    while (lhs_start > 0 && (std::isalnum(static_cast<unsigned char>(result[lhs_start - 1])) ||
                           result[lhs_start - 1] == '_' || result[lhs_start - 1] == '.')) {
                        lhs_start--;
                    }
                }
                std::string lhs = result.substr(lhs_start, p - lhs_start + 1);

                size_t rhs_start = i + 2;
                while (rhs_start < n && (source[rhs_start] == ' ' || source[rhs_start] == '\t')) {
                    rhs_start++;
                }
                size_t rhs_end = rhs_start;
                if (rhs_end < n && source[rhs_end] == '(') {
                    int depth = 1;
                    rhs_end++;
                    while (rhs_end < n && depth > 0) {
                        if (source[rhs_end] == '(') depth++;
                        else if (source[rhs_end] == ')') depth--;
                        rhs_end++;
                    }
                } else {
                    while (rhs_end < n && (std::isalnum(static_cast<unsigned char>(source[rhs_end])) ||
                           source[rhs_end] == '_' || source[rhs_end] == '.')) {
                        rhs_end++;
                    }
                    if (rhs_end < n && (source[rhs_end] == '(' || source[rhs_end] == '[')) {
                        char openChar = source[rhs_end];
                        char closeChar = (openChar == '(') ? ')' : ']';
                        int depth = 1;
                        rhs_end++;
                        while (rhs_end < n && depth > 0) {
                            if (source[rhs_end] == openChar) depth++;
                            else if (source[rhs_end] == closeChar) depth--;
                            rhs_end++;
                        }
                    }
                }
                std::string rhs = source.substr(rhs_start, rhs_end - rhs_start);
                result.erase(lhs_start);
                result += "math.floor((" + lhs + ") / (" + rhs + "))";
                i = rhs_end;
                continue;
            }
        }

        if (std::isdigit(static_cast<unsigned char>(source[i])) ||
            (source[i] == '.' && i + 1 < n && std::isdigit(static_cast<unsigned char>(source[i + 1])))) {
            if (source[i] == '0' && i + 1 < n && (source[i + 1] == 'x' || source[i + 1] == 'X')) {
                result += source[i++];
                result += source[i++];
                bool has_dot = false;
                while (i < n) {
                    if (std::isxdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    } else if (source[i] == '.' && !has_dot && (i + 1 == n || source[i + 1] != '.')) {
                        has_dot = true;
                        result += source[i++];
                    } else {
                        break;
                    }
                }
                if (i < n && (source[i] == 'p' || source[i] == 'P')) {
                    result += source[i++];
                    if (i < n && (source[i] == '+' || source[i] == '-')) {
                        result += source[i++];
                    }
                    while (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    }
                }
            } else {
                bool has_dot = false;
                while (i < n) {
                    if (std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    } else if (source[i] == '.' && !has_dot && (i + 1 == n || source[i + 1] != '.')) {
                        has_dot = true;
                        result += source[i++];
                    } else {
                        break;
                    }
                }
                if (i < n && (source[i] == 'e' || source[i] == 'E')) {
                    result += source[i++];
                    if (i < n && (source[i] == '+' || source[i] == '-')) {
                        result += source[i++];
                    }
                    while (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    }
                }
            }
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(source[i])) || source[i] == '_') {
            size_t start = i;
            while (i < n && (std::isalnum(static_cast<unsigned char>(source[i])) || source[i] == '_')) {
                i++;
            }
            std::string ident = source.substr(start, i - start);

            bool isMemberAccess = false;
            size_t p = result.find_last_not_of(" \t\r\n");
            if (p != std::string::npos) {
                if (result[p] == ':') {
                    isMemberAccess = true;
                } else if (result[p] == '.') {
                    if (p == 0 || result[p - 1] != '.') {
                        isMemberAccess = true;
                    }
                }
            }

            bool isTableKey = false;
            size_t next_p = i;
            while (next_p < n && (source[next_p] == ' ' || source[next_p] == '\t' || source[next_p] == '\r' || source[next_p] == '\n')) {
                next_p++;
            }
            if (next_p < n && source[next_p] == '=' && (next_p + 1 == n || source[next_p + 1] != '=')) {
                isTableKey = true;
            }

            if (!isMemberAccess && !isTableKey) {
                const auto& map = getKeywordMap();
                auto it = map.find(ident);
                if (it != map.end()) {
                    result += it->second;
                } else {
                    result += ident;
                }
            } else {
                result += ident;
            }
            continue;
        }

        result += source[i++];
    }

    return result;
}

// ---------------------------------------------------------------------------
// Konversi penuh: Luai -> Lua Standar
// ---------------------------------------------------------------------------
std::string Lexer::toLua(const std::string& source) {
    std::string result;
    size_t n = source.size();
    size_t i = 0;

    if (n >= 3 && static_cast<unsigned char>(source[0]) == 0xEF &&
        static_cast<unsigned char>(source[1]) == 0xBB &&
        static_cast<unsigned char>(source[2]) == 0xBF) {
        i = 3;
    }

    const auto& kwMap = getKeywordMap();
    const auto& globalMap = getLuaiToLuaGlobalMap();
    const auto& moduleMap = getLuaiToLuaModuleMap();
    const auto& memberMap = getLuaiToLuaMemberMap();
    const auto& methodMap = getLuaiToLuaMethodMap();

    while (i < n) {
        // Shebang
        if (i == 0 && source[i] == '#') {
            while (i < n && source[i] != '\n') {
                result += source[i++];
            }
            continue;
        }

        // Komentar
        if (i + 1 < n && source[i] == '-' && source[i + 1] == '-') {
            size_t j = i + 2;
            if (j < n && source[j] == '[') {
                size_t k = j + 1;
                size_t eq_count = 0;
                while (k < n && source[k] == '=') {
                    eq_count++;
                    k++;
                }
                if (k < n && source[k] == '[') {
                    std::string close_tag = "]" + std::string(eq_count, '=') + "]";
                    size_t close_pos = source.find(close_tag, k + 1);
                    if (close_pos != std::string::npos) {
                        size_t end_pos = close_pos + close_tag.size();
                        result += source.substr(i, end_pos - i);
                        i = end_pos;
                    } else {
                        result += source.substr(i);
                        i = n;
                    }
                    continue;
                }
            }
            while (i < n && source[i] != '\n') {
                result += source[i++];
            }
            continue;
        }

        // String literal
        if (source[i] == '"' || source[i] == '\'') {
            char quote = source[i];
            result += quote;
            i++;
            while (i < n) {
                char sc = source[i];
                if (sc == '\\') {
                    result += sc;
                    i++;
                    if (i < n) {
                        result += source[i++];
                    }
                } else if (sc == quote) {
                    result += sc;
                    i++;
                    break;
                } else {
                    result += sc;
                    i++;
                }
            }
            continue;
        }

        // Multiline string [[ ... ]]
        if (source[i] == '[') {
            size_t j = i + 1;
            size_t eq_count = 0;
            while (j < n && source[j] == '=') {
                eq_count++;
                j++;
            }
            if (j < n && source[j] == '[') {
                std::string close_tag = "]" + std::string(eq_count, '=') + "]";
                size_t close_pos = source.find(close_tag, j + 1);
                if (close_pos != std::string::npos) {
                    size_t end_pos = close_pos + close_tag.size();
                    result += source.substr(i, end_pos - i);
                    i = end_pos;
                } else {
                    result += source.substr(i);
                    i = n;
                }
                continue;
            }
        }

        // Concat .. dan vararg ...
        if (source[i] == '.' && i + 1 < n && source[i + 1] == '.') {
            if (i + 2 < n && source[i + 2] == '.') {
                result += "...";
                i += 3;
                continue;
            }
            if (!result.empty() && std::isdigit(static_cast<unsigned char>(result.back()))) {
                result += ' ';
            }
            result += "..";
            i += 2;
            if (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                result += ' ';
            }
            continue;
        }

        // Pembagian bulat // -> math.floor((a)/(b)) untuk kompatibilitas ke semua versi Lua
        if (source[i] == '/' && i + 1 < n && source[i + 1] == '/') {
            size_t p = result.find_last_not_of(" \t\r\n");
            if (p != std::string::npos) {
                size_t lhs_start = p;
                if (result[p] == ')') {
                    int depth = 1;
                    while (lhs_start > 0 && depth > 0) {
                        lhs_start--;
                        if (result[lhs_start] == ')') depth++;
                        else if (result[lhs_start] == '(') depth--;
                    }
                    while (lhs_start > 0 && (std::isalnum(static_cast<unsigned char>(result[lhs_start - 1])) ||
                           result[lhs_start - 1] == '_' || result[lhs_start - 1] == '.' || result[lhs_start - 1] == ':')) {
                        lhs_start--;
                    }
                } else if (result[p] == ']') {
                    int depth = 1;
                    while (lhs_start > 0 && depth > 0) {
                        lhs_start--;
                        if (result[lhs_start] == ']') depth++;
                        else if (result[lhs_start] == '[') depth--;
                    }
                    while (lhs_start > 0 && (std::isalnum(static_cast<unsigned char>(result[lhs_start - 1])) ||
                           result[lhs_start - 1] == '_' || result[lhs_start - 1] == '.' || result[lhs_start - 1] == ':')) {
                        lhs_start--;
                    }
                } else {
                    while (lhs_start > 0 && (std::isalnum(static_cast<unsigned char>(result[lhs_start - 1])) ||
                           result[lhs_start - 1] == '_' || result[lhs_start - 1] == '.')) {
                        lhs_start--;
                    }
                }
                std::string lhs = result.substr(lhs_start, p - lhs_start + 1);

                size_t rhs_start = i + 2;
                while (rhs_start < n && (source[rhs_start] == ' ' || source[rhs_start] == '\t')) {
                    rhs_start++;
                }
                size_t rhs_end = rhs_start;
                if (rhs_end < n && source[rhs_end] == '(') {
                    int depth = 1;
                    rhs_end++;
                    while (rhs_end < n && depth > 0) {
                        if (source[rhs_end] == '(') depth++;
                        else if (source[rhs_end] == ')') depth--;
                        rhs_end++;
                    }
                } else {
                    while (rhs_end < n && (std::isalnum(static_cast<unsigned char>(source[rhs_end])) ||
                           source[rhs_end] == '_' || source[rhs_end] == '.')) {
                        rhs_end++;
                    }
                    if (rhs_end < n && (source[rhs_end] == '(' || source[rhs_end] == '[')) {
                        char openChar = source[rhs_end];
                        char closeChar = (openChar == '(') ? ')' : ']';
                        int depth = 1;
                        rhs_end++;
                        while (rhs_end < n && depth > 0) {
                            if (source[rhs_end] == openChar) depth++;
                            else if (source[rhs_end] == closeChar) depth--;
                            rhs_end++;
                        }
                    }
                }
                std::string rhs = source.substr(rhs_start, rhs_end - rhs_start);
                result.erase(lhs_start);
                result += "math.floor((" + lhs + ") / (" + rhs + "))";
                i = rhs_end;
                continue;
            }
        }

        // Angka literal
        if (std::isdigit(static_cast<unsigned char>(source[i])) ||
            (source[i] == '.' && i + 1 < n && std::isdigit(static_cast<unsigned char>(source[i + 1])))) {
            if (source[i] == '0' && i + 1 < n && (source[i + 1] == 'x' || source[i + 1] == 'X')) {
                result += source[i++];
                result += source[i++];
                bool has_dot = false;
                while (i < n) {
                    if (std::isxdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    } else if (source[i] == '.' && !has_dot && (i + 1 == n || source[i + 1] != '.')) {
                        has_dot = true;
                        result += source[i++];
                    } else {
                        break;
                    }
                }
                if (i < n && (source[i] == 'p' || source[i] == 'P')) {
                    result += source[i++];
                    if (i < n && (source[i] == '+' || source[i] == '-')) {
                        result += source[i++];
                    }
                    while (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    }
                }
            } else {
                bool has_dot = false;
                while (i < n) {
                    if (std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    } else if (source[i] == '.' && !has_dot && (i + 1 == n || source[i + 1] != '.')) {
                        has_dot = true;
                        result += source[i++];
                    } else {
                        break;
                    }
                }
                if (i < n && (source[i] == 'e' || source[i] == 'E')) {
                    result += source[i++];
                    if (i < n && (source[i] == '+' || source[i] == '-')) {
                        result += source[i++];
                    }
                    while (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    }
                }
            }
            continue;
        }

        // Identifiers
        if (std::isalpha(static_cast<unsigned char>(source[i])) || source[i] == '_') {
            size_t start = i;
            while (i < n && (std::isalnum(static_cast<unsigned char>(source[i])) || source[i] == '_')) {
                i++;
            }
            std::string ident = source.substr(start, i - start);

            // Cek akses member: .ident atau :ident
            bool isDotAccess = false;
            bool isColonAccess = false;
            std::string parentObj;

            size_t p = result.find_last_not_of(" \t\r\n");
            if (p != std::string::npos) {
                if (result[p] == ':') {
                    isColonAccess = true;
                } else if (result[p] == '.') {
                    if (p == 0 || result[p - 1] != '.') {
                        isDotAccess = true;
                        // Ambil nama parent object sebelum titik
                        size_t obj_end = result.find_last_not_of(" \t\r\n", p - 1);
                        if (obj_end != std::string::npos) {
                            size_t obj_start = obj_end;
                            while (obj_start > 0 && (std::isalnum(static_cast<unsigned char>(result[obj_start - 1])) || result[obj_start - 1] == '_')) {
                                obj_start--;
                            }
                            parentObj = result.substr(obj_start, obj_end - obj_start + 1);
                        }
                    }
                }
            }

            // Cek apakah key pada tabel: ident =
            bool isTableKey = false;
            size_t next_p = i;
            while (next_p < n && (source[next_p] == ' ' || source[next_p] == '\t' || source[next_p] == '\r' || source[next_p] == '\n')) {
                next_p++;
            }
            if (next_p < n && source[next_p] == '=' && (next_p + 1 == n || source[next_p + 1] != '=')) {
                isTableKey = true;
            }

            if (isColonAccess) {
                auto methodIt = methodMap.find(ident);
                if (methodIt != methodMap.end()) {
                    result += methodIt->second;
                    continue;
                }
                result += ident;
            } else if (isDotAccess && !parentObj.empty()) {
                // Cek apakah parentObj adalah modul standar yang dikenali
                auto modIt = memberMap.find(parentObj);
                if (modIt != memberMap.end()) {
                    auto memIt = modIt->second.find(ident);
                    if (memIt != modIt->second.end()) {
                        result += memIt->second;
                        continue;
                    }
                }
                result += ident;
            } else if (!isDotAccess && !isColonAccess && !isTableKey) {
                // 1. Cek kata kunci Luai -> Lua
                auto kwIt = kwMap.find(ident);
                if (kwIt != kwMap.end()) {
                    result += kwIt->second;
                    continue;
                }

                // 2. Cek modul Luai -> Lua (misal matematika -> math)
                auto modIt = moduleMap.find(ident);
                if (modIt != moduleMap.end()) {
                    result += modIt->second;
                    continue;
                }

                // 3. Cek fungsi global Luai -> Lua (misal cetak -> print)
                auto gIt = globalMap.find(ident);
                if (gIt != globalMap.end()) {
                    result += gIt->second;
                    continue;
                }

                result += ident;
            } else {
                result += ident;
            }
            continue;
        }

        result += source[i++];
    }

    return result;
}

// ---------------------------------------------------------------------------
// Konversi penuh: Lua Standar -> Luai
// ---------------------------------------------------------------------------
std::string Lexer::toLuai(const std::string& source) {
    std::string result;
    size_t n = source.size();
    size_t i = 0;

    if (n >= 3 && static_cast<unsigned char>(source[0]) == 0xEF &&
        static_cast<unsigned char>(source[1]) == 0xBB &&
        static_cast<unsigned char>(source[2]) == 0xBF) {
        i = 3;
    }

    const auto& revKwMap = getReverseKeywordMap();
    const auto& globalMap = getLuaToLuaiGlobalMap();
    const auto& moduleMap = getLuaToLuaiModuleMap();
    const auto& memberMap = getLuaToLuaiMemberMap();
    const auto& methodMap = getLuaToLuaiMethodMap();

    while (i < n) {
        // Shebang
        if (i == 0 && source[i] == '#') {
            while (i < n && source[i] != '\n') {
                result += source[i++];
            }
            continue;
        }

        // Komentar
        if (i + 1 < n && source[i] == '-' && source[i + 1] == '-') {
            size_t j = i + 2;
            if (j < n && source[j] == '[') {
                size_t k = j + 1;
                size_t eq_count = 0;
                while (k < n && source[k] == '=') {
                    eq_count++;
                    k++;
                }
                if (k < n && source[k] == '[') {
                    std::string close_tag = "]" + std::string(eq_count, '=') + "]";
                    size_t close_pos = source.find(close_tag, k + 1);
                    if (close_pos != std::string::npos) {
                        size_t end_pos = close_pos + close_tag.size();
                        result += source.substr(i, end_pos - i);
                        i = end_pos;
                    } else {
                        result += source.substr(i);
                        i = n;
                    }
                    continue;
                }
            }
            while (i < n && source[i] != '\n') {
                result += source[i++];
            }
            continue;
        }

        // String literal
        if (source[i] == '"' || source[i] == '\'') {
            char quote = source[i];
            result += quote;
            i++;
            while (i < n) {
                char sc = source[i];
                if (sc == '\\') {
                    result += sc;
                    i++;
                    if (i < n) {
                        result += source[i++];
                    }
                } else if (sc == quote) {
                    result += sc;
                    i++;
                    break;
                } else {
                    result += sc;
                    i++;
                }
            }
            continue;
        }

        // Multiline string [[ ... ]]
        if (source[i] == '[') {
            size_t j = i + 1;
            size_t eq_count = 0;
            while (j < n && source[j] == '=') {
                eq_count++;
                j++;
            }
            if (j < n && source[j] == '[') {
                std::string close_tag = "]" + std::string(eq_count, '=') + "]";
                size_t close_pos = source.find(close_tag, j + 1);
                if (close_pos != std::string::npos) {
                    size_t end_pos = close_pos + close_tag.size();
                    result += source.substr(i, end_pos - i);
                    i = end_pos;
                } else {
                    result += source.substr(i);
                    i = n;
                }
                continue;
            }
        }

        // Concat .. dan vararg ...
        if (source[i] == '.' && i + 1 < n && source[i + 1] == '.') {
            if (i + 2 < n && source[i + 2] == '.') {
                result += "...";
                i += 3;
                continue;
            }
            if (!result.empty() && std::isdigit(static_cast<unsigned char>(result.back()))) {
                result += ' ';
            }
            result += "..";
            i += 2;
            if (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                result += ' ';
            }
            continue;
        }

        // Angka literal
        if (std::isdigit(static_cast<unsigned char>(source[i])) ||
            (source[i] == '.' && i + 1 < n && std::isdigit(static_cast<unsigned char>(source[i + 1])))) {
            if (source[i] == '0' && i + 1 < n && (source[i + 1] == 'x' || source[i + 1] == 'X')) {
                result += source[i++];
                result += source[i++];
                bool has_dot = false;
                while (i < n) {
                    if (std::isxdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    } else if (source[i] == '.' && !has_dot && (i + 1 == n || source[i + 1] != '.')) {
                        has_dot = true;
                        result += source[i++];
                    } else {
                        break;
                    }
                }
                if (i < n && (source[i] == 'p' || source[i] == 'P')) {
                    result += source[i++];
                    if (i < n && (source[i] == '+' || source[i] == '-')) {
                        result += source[i++];
                    }
                    while (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    }
                }
            } else {
                bool has_dot = false;
                while (i < n) {
                    if (std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    } else if (source[i] == '.' && !has_dot && (i + 1 == n || source[i + 1] != '.')) {
                        has_dot = true;
                        result += source[i++];
                    } else {
                        break;
                    }
                }
                if (i < n && (source[i] == 'e' || source[i] == 'E')) {
                    result += source[i++];
                    if (i < n && (source[i] == '+' || source[i] == '-')) {
                        result += source[i++];
                    }
                    while (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i++];
                    }
                }
            }
            continue;
        }

        // Identifiers
        if (std::isalpha(static_cast<unsigned char>(source[i])) || source[i] == '_') {
            size_t start = i;
            while (i < n && (std::isalnum(static_cast<unsigned char>(source[i])) || source[i] == '_')) {
                i++;
            }
            std::string ident = source.substr(start, i - start);

            // Cek akses member: .ident atau :ident
            bool isDotAccess = false;
            bool isColonAccess = false;
            std::string parentObj;

            size_t p = result.find_last_not_of(" \t\r\n");
            if (p != std::string::npos) {
                if (result[p] == ':') {
                    isColonAccess = true;
                } else if (result[p] == '.') {
                    if (p == 0 || result[p - 1] != '.') {
                        isDotAccess = true;
                        // Ambil nama parent object sebelum titik (yang mungkin sudah diubah ke Luai, misal matematika)
                        size_t obj_end = result.find_last_not_of(" \t\r\n", p - 1);
                        if (obj_end != std::string::npos) {
                            size_t obj_start = obj_end;
                            while (obj_start > 0 && (std::isalnum(static_cast<unsigned char>(result[obj_start - 1])) || result[obj_start - 1] == '_')) {
                                obj_start--;
                            }
                            parentObj = result.substr(obj_start, obj_end - obj_start + 1);
                        }
                    }
                }
            }

            // Cek apakah key pada tabel: ident =
            bool isTableKey = false;
            size_t next_p = i;
            while (next_p < n && (source[next_p] == ' ' || source[next_p] == '\t' || source[next_p] == '\r' || source[next_p] == '\n')) {
                next_p++;
            }
            if (next_p < n && source[next_p] == '=' && (next_p + 1 == n || source[next_p + 1] != '=')) {
                isTableKey = true;
            }

            if (isColonAccess) {
                auto methodIt = methodMap.find(ident);
                if (methodIt != methodMap.end()) {
                    result += methodIt->second;
                    continue;
                }
                result += ident;
            } else if (isDotAccess && !parentObj.empty()) {
                // Cek apakah parentObj adalah modul standar Luai
                auto modIt = memberMap.find(parentObj);
                if (modIt != memberMap.end()) {
                    auto memIt = modIt->second.find(ident);
                    if (memIt != modIt->second.end()) {
                        result += memIt->second;
                        continue;
                    }
                }
                result += ident;
            } else if (!isDotAccess && !isColonAccess && !isTableKey) {
                // 1. Cek kata kunci Lua -> Luai
                auto kwIt = revKwMap.find(ident);
                if (kwIt != revKwMap.end()) {
                    result += kwIt->second;
                    continue;
                }

                // 2. Cek modul Lua -> Luai (misal math -> matematika)
                auto modIt = moduleMap.find(ident);
                if (modIt != moduleMap.end()) {
                    result += modIt->second;
                    continue;
                }

                // 3. Cek fungsi global Lua -> Luai (misal print -> cetak)
                auto gIt = globalMap.find(ident);
                if (gIt != globalMap.end()) {
                    result += gIt->second;
                    continue;
                }

                result += ident;
            } else {
                result += ident;
            }
            continue;
        }

        result += source[i++];
    }

    return result;
}
