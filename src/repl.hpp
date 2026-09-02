#ifndef LUAI_REPL_HPP
#define LUAI_REPL_HPP

#include "runtime.hpp"
#include <string>

class Repl {
public:
    static void run(LuaiRuntime& runtime);
private:
    static std::string trim(const std::string& str);
    static bool isIncompleteChunk(const std::string& errorMsg);
};

#endif
