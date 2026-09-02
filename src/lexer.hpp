#ifndef LUAI_LEXER_HPP
#define LUAI_LEXER_HPP

#include <string>
#include <unordered_map>

class Lexer {
public:
    static std::string transpile(const std::string& source);
    static bool isKeyword(const std::string& word);
private:
    static const std::unordered_map<std::string, std::string>& getKeywordMap();
};

#endif
