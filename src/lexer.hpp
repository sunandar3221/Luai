#ifndef LUAI_LEXER_HPP
#define LUAI_LEXER_HPP

#include <string>
#include <unordered_map>

class Lexer {
public:
    // Transpile Luai ke Lua untuk eksekusi mesin runtime internal
    static std::string transpile(const std::string& source);

    // Konversi penuh dari kode Luai ke Lua standar (termasuk fungsi & modul bawaan)
    static std::string toLua(const std::string& source);

    // Konversi penuh dari kode Lua standar ke Luai (kata kunci, fungsi & modul ke Bahasa Indonesia)
    static std::string toLuai(const std::string& source);

    // Mengecek apakah suatu kata adalah kata kunci Luai
    static bool isKeyword(const std::string& word);

    // Mengecek apakah suatu kata adalah kata kunci Lua standar
    static bool isLuaKeyword(const std::string& word);

private:
    static const std::unordered_map<std::string, std::string>& getKeywordMap();
    static const std::unordered_map<std::string, std::string>& getReverseKeywordMap();
    static const std::unordered_map<std::string, std::string>& getLuaiToLuaGlobalMap();
    static const std::unordered_map<std::string, std::string>& getLuaToLuaiGlobalMap();
    static const std::unordered_map<std::string, std::string>& getLuaiToLuaModuleMap();
    static const std::unordered_map<std::string, std::string>& getLuaToLuaiModuleMap();
    static const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& getLuaiToLuaMemberMap();
    static const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& getLuaToLuaiMemberMap();
    static const std::unordered_map<std::string, std::string>& getLuaiToLuaMethodMap();
    static const std::unordered_map<std::string, std::string>& getLuaToLuaiMethodMap();
};

#endif
