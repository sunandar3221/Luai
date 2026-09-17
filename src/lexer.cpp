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
            // Find left operand from result
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

                // Find right operand from source
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
                result += source[i];
                i++;
                result += source[i];
                i++;
                bool has_dot = false;
                while (i < n) {
                    if (std::isxdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i];
                        i++;
                    } else if (source[i] == '.' && !has_dot && (i + 1 == n || source[i + 1] != '.')) {
                        has_dot = true;
                        result += source[i];
                        i++;
                    } else {
                        break;
                    }
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
                bool has_dot = false;
                while (i < n) {
                    if (std::isdigit(static_cast<unsigned char>(source[i]))) {
                        result += source[i];
                        i++;
                    } else if (source[i] == '.' && !has_dot && (i + 1 == n || source[i + 1] != '.')) {
                        has_dot = true;
                        result += source[i];
                        i++;
                    } else {
                        break;
                    }
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

            // Check if this identifier is preceded by '.' or ':' (member access/method call)
            bool isMemberAccess = false;
            size_t p = result.find_last_not_of(" \t\r\n");
            if (p != std::string::npos) {
                if (result[p] == ':') {
                    isMemberAccess = true;
                } else if (result[p] == '.') {
                    // Make sure it's a single '.', not '..' or '...'
                    if (p == 0 || result[p - 1] != '.') {
                        isMemberAccess = true;
                    }
                }
            }

            // Check if this identifier is followed by '=' (table key in table constructor, e.g. { selesai = ... })
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

        result += source[i];
        i++;
    }

    return result;
}
