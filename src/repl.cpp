#include "repl.hpp"
#include "lexer.hpp"
#include "../lua-5.4.7/src/lua.hpp"
#include <iostream>
#include <sstream>

std::string Repl::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool Repl::isIncompleteChunk(const std::string& errorMsg) {
    return errorMsg.find("<eof>") != std::string::npos ||
           errorMsg.find("near '<eof>'") != std::string::npos ||
           errorMsg.find("unfinished string") != std::string::npos;
}

void Repl::run(LuaiRuntime& runtime) {
    std::cout << "Luai 1.0.0 (Lua dialek Bahasa Indonesia) [Lua 5.4.7]\n";
    std::cout << "Ketik \"keluar\" atau \"exit\" untuk mengakhiri sesi interaktif.\n\n";

    std::string buffer;
    while (true) {
        if (buffer.empty()) {
            std::cout << "luai> ";
        } else {
            std::cout << ">> ";
        }
        std::cout.flush();

        std::string line;
        if (!std::getline(std::cin, line)) {
            break;
        }

        std::string trimmed = trim(line);
        if (buffer.empty() && (trimmed == "keluar" || trimmed == "exit")) {
            break;
        }

        if (buffer.empty() && trimmed.empty()) {
            continue;
        }

        if (!buffer.empty()) {
            buffer += "\n";
        }
        buffer += line;

        std::string transpiled = Lexer::transpile(buffer);
        std::string retCode = "return " + transpiled;

        lua_State* L = runtime.getState();
        int status = luaL_loadbuffer(L, retCode.data(), retCode.size(), "=luai");
        if (status == LUA_OK) {
            int base = lua_gettop(L) - 1;
            if (lua_pcall(L, 0, LUA_MULTRET, 0) == LUA_OK) {
                int top = lua_gettop(L);
                int nres = top - base;
                for (int i = 1; i <= nres; i++) {
                    if (i > 1) std::cout << "\t";
                    int idx = base + i;
                    if (lua_isboolean(L, idx)) {
                        std::cout << (lua_toboolean(L, idx) ? "benar" : "salah");
                    } else if (lua_isnil(L, idx)) {
                        std::cout << "nihil";
                    } else {
                        const char* s = luaL_tolstring(L, idx, nullptr);
                        std::cout << (s ? s : "");
                        lua_pop(L, 1);
                    }
                }
                if (nres > 0) std::cout << "\n";
                lua_settop(L, base);
            } else {
                const char* err = lua_tostring(L, -1);
                std::cerr << (err ? err : "Galat tidak dikenal") << "\n";
                lua_pop(L, 1);
            }
            buffer.clear();
            continue;
        }

        lua_pop(L, 1);

        status = luaL_loadbuffer(L, transpiled.data(), transpiled.size(), "=luai");
        if (status == LUA_OK) {
            if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
                const char* err = lua_tostring(L, -1);
                std::cerr << (err ? err : "Galat tidak dikenal") << "\n";
                lua_pop(L, 1);
            }
            buffer.clear();
            continue;
        }

        std::string err = lua_tostring(L, -1);
        lua_pop(L, 1);

        if (isIncompleteChunk(err)) {
            continue;
        }

        std::cerr << err << "\n";
        buffer.clear();
    }
}
