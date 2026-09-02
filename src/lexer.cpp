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

bool Lexer::isKeyword(const std::string& word) {
    const auto& map = getKeywordMap();
    return map.find(word) != map.end();
}

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

        if (std::isdigit(static_cast<unsigned char>(source[i])) ||
            (source[i] == '.' && i + 1 < n && std::isdigit(static_cast<unsigned char>(source[i + 1])))) {
            if (source[i] == '0' && i + 1 < n && (source[i + 1] == 'x' || source[i + 1] == 'X')) {
                result += source[i];
                i++;
                result += source[i];
                i++;
                while (i < n && (std::isxdigit(static_cast<unsigned char>(source[i])) || source[i] == '.')) {
                    result += source[i];
                    i++;
                }
                if (i < n && (source[i] == 'p' || source[i] == 'P')) {
                    result += source[i];
                    i++;
                    if (i < n && (source[i] == '+' || source[i] == '-')) {
                        result += source[i];
                        i++;
                    }
                    while (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i];
                        i++;
                    }
                }
            } else {
                while (i < n && (std::isdigit(static_cast<unsigned char>(source[i])) || source[i] == '.')) {
                    result += source[i];
                    i++;
                }
                if (i < n && (source[i] == 'e' || source[i] == 'E')) {
                    result += source[i];
                    i++;
                    if (i < n && (source[i] == '+' || source[i] == '-')) {
                        result += source[i];
                        i++;
                    }
                    while (i < n && std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i];
                        i++;
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
            const auto& map = getKeywordMap();
            auto it = map.find(ident);
            if (it != map.end()) {
                result += it->second;
            } else {
                result += ident;
            }
            continue;
        }

        result += source[i];
        i++;
    }

    return result;
}
