#include "repl.hpp"
#include "lexer.hpp"

#if defined(USE_LUA54)
#include "lua.hpp"
#elif __has_include("../luajit/src/lua.hpp")
#include "../luajit/src/lua.hpp"
#elif __has_include("luajit.h")
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "luajit.h"
}
#elif __has_include("lua.hpp")
#include "lua.hpp"
#else
#include "../lua-5.4.7/src/lua.hpp"
#endif

#include <iostream>
#include <sstream>

#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 502
static const char* luai_tolstring(lua_State* L, int idx, size_t* len) {
    if (luaL_callmeta(L, idx, "__tostring")) {
        if (!lua_isstring(L, -1))
            luaL_error(L, "'__tostring' must return a string");
    } else {
        switch (lua_type(L, idx)) {
            case LUA_TNUMBER:
            case LUA_TSTRING:
                lua_pushvalue(L, idx);
                break;
            case LUA_TBOOLEAN:
                lua_pushstring(L, lua_toboolean(L, idx) ? "true" : "false");
                break;
            case LUA_TNIL:
                lua_pushliteral(L, "nil");
                break;
            default: {
                char buf[64];
                snprintf(buf, sizeof(buf), "%s: %p", lua_typename(L, lua_type(L, idx)), lua_topointer(L, idx));
                lua_pushstring(L, buf);
                break;
            }
        }
    }
    return lua_tolstring(L, -1, len);
}
#ifndef luaL_tolstring
#define luaL_tolstring luai_tolstring
#endif
#endif

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
    std::cout << "Luai 1.0.0 (Dialek Bahasa Indonesia) - engine " << luaiEngineName()
              << " [" << (luaiJitEnabled(runtime.getState()) ? "JIT aktif" : "tanpa JIT") << "]\n";
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
