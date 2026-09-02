#include "runtime.hpp"
#include "lexer.hpp"
#include "../lua-5.4.7/src/lua.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>

static int luai_cetak(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (i > 1) {
            std::cout << "\t";
        }
        if (lua_isboolean(L, i)) {
            std::cout << (lua_toboolean(L, i) ? "benar" : "salah");
        } else if (lua_isnil(L, i)) {
            std::cout << "nihil";
        } else {
            const char* s = luaL_tolstring(L, i, nullptr);
            std::cout << (s ? s : "");
            lua_pop(L, 1);
        }
    }
    std::cout << "\n";
    std::cout.flush();
    return 0;
}

static int luai_tipe(lua_State* L) {
    luaL_checkany(L, 1);
    int t = lua_type(L, 1);
    switch (t) {
        case LUA_TNIL:
            lua_pushstring(L, "nihil");
            break;
        case LUA_TNUMBER:
            lua_pushstring(L, "angka");
            break;
        case LUA_TBOOLEAN:
            lua_pushstring(L, "boolean");
            break;
        case LUA_TSTRING:
            lua_pushstring(L, "teks");
            break;
        case LUA_TTABLE:
            lua_pushstring(L, "tabel");
            break;
        case LUA_TFUNCTION:
            lua_pushstring(L, "fungsi");
            break;
        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
            lua_pushstring(L, "userdata");
            break;
        case LUA_TTHREAD:
            lua_pushstring(L, "korutin");
            break;
        default:
            lua_pushstring(L, "tidak_dikenal");
            break;
    }
    return 1;
}

static int luai_ke_angka(lua_State* L) {
    int base = (int)luaL_optinteger(L, 2, 10);
    if (base == 10) {
        int isnum = 0;
        lua_Number n = lua_tonumberx(L, 1, &isnum);
        if (isnum) {
            lua_pushnumber(L, n);
            return 1;
        }
        if (lua_isstring(L, 1)) {
            size_t len = 0;
            const char* s = lua_tolstring(L, 1, &len);
            char* endptr = nullptr;
            double d = std::strtod(s, &endptr);
            if (endptr != s && *endptr == '\0') {
                lua_pushnumber(L, d);
                return 1;
            }
        }
    } else {
        const char* s = luaL_checkstring(L, 1);
        char* endptr = nullptr;
        long long val = std::strtoll(s, &endptr, base);
        if (endptr != s && *endptr == '\0') {
            lua_pushinteger(L, val);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

static int luai_ke_teks(lua_State* L) {
    luaL_checkany(L, 1);
    if (lua_isboolean(L, 1)) {
        lua_pushstring(L, lua_toboolean(L, 1) ? "benar" : "salah");
        return 1;
    }
    if (lua_isnil(L, 1)) {
        lua_pushstring(L, "nihil");
        return 1;
    }
    luaL_tolstring(L, 1, nullptr);
    return 1;
}

static int luai_pasangan(lua_State* L) {
    luaL_checkany(L, 1);
    lua_getglobal(L, "pairs");
    lua_pushvalue(L, 1);
    lua_call(L, 1, 3);
    return 3;
}

static int luai_i_pasangan(lua_State* L) {
    luaL_checkany(L, 1);
    lua_getglobal(L, "ipairs");
    lua_pushvalue(L, 1);
    lua_call(L, 1, 3);
    return 3;
}

static int luai_masukan(lua_State* L) {
    int top = lua_gettop(L);
    if (top >= 1 && lua_isstring(L, 1)) {
        const char* str = lua_tostring(L, 1);
        if (str && str[0] != '*') {
            std::cout << str;
            std::cout.flush();
            std::string line;
            if (std::getline(std::cin, line)) {
                lua_pushlstring(L, line.data(), line.size());
                return 1;
            }
            lua_pushnil(L);
            return 1;
        }
    }
    if (top == 0) {
        std::string line;
        if (std::getline(std::cin, line)) {
            lua_pushlstring(L, line.data(), line.size());
            return 1;
        }
        lua_pushnil(L);
        return 1;
    }
    lua_getglobal(L, "io");
    lua_getfield(L, -1, "read");
    for (int i = 1; i <= top; i++) {
        lua_pushvalue(L, i);
    }
    lua_call(L, top, 1);
    return 1;
}

static int luai_lepas(lua_State* L) {
    lua_getglobal(L, "table");
    lua_getfield(L, -1, "unpack");
    int top = lua_gettop(L) - 2;
    for (int i = 1; i <= top; i++) {
        lua_pushvalue(L, i);
    }
    lua_call(L, top, LUA_MULTRET);
    return lua_gettop(L) - (top + 2);
}

LuaiRuntime::LuaiRuntime() : L(nullptr) {}

LuaiRuntime::~LuaiRuntime() {
    if (L) {
        lua_close(L);
        L = nullptr;
    }
}

bool LuaiRuntime::init() {
    L = luaL_newstate();
    if (!L) {
        lastError = "Gagal menginisialisasi Lua state";
        return false;
    }

    luaL_openlibs(L);
    registerIndonesianBindings();
    registerModuleAliases();
    registerSearcher();

    return true;
}

void LuaiRuntime::registerIndonesianBindings() {
    lua_pushcfunction(L, luai_cetak);
    lua_setglobal(L, "cetak");

    lua_pushcfunction(L, luai_tipe);
    lua_setglobal(L, "tipe");

    lua_pushcfunction(L, luai_ke_angka);
    lua_setglobal(L, "ke_angka");

    lua_pushcfunction(L, luai_ke_teks);
    lua_setglobal(L, "ke_teks");

    lua_pushcfunction(L, luai_pasangan);
    lua_setglobal(L, "pasangan");

    lua_pushcfunction(L, luai_i_pasangan);
    lua_setglobal(L, "i_pasangan");

    lua_pushcfunction(L, luai_masukan);
    lua_setglobal(L, "masukan");

    lua_pushcfunction(L, luai_lepas);
    lua_setglobal(L, "lepas");

    auto aliasGlobal = [this](const char* src, const char* dst) {
        lua_getglobal(L, src);
        lua_setglobal(L, dst);
    };

    aliasGlobal("assert", "tegaskan");
    aliasGlobal("error", "kesalahan");
    aliasGlobal("pcall", "pcall_aman");
    aliasGlobal("pcall", "panggil_aman");
    aliasGlobal("xpcall", "xpcall_aman");
    aliasGlobal("setmetatable", "set_metatabel");
    aliasGlobal("getmetatable", "ambil_metatabel");
    aliasGlobal("collectgarbage", "koleksi_sampah");
    aliasGlobal("select", "pilih");
    aliasGlobal("load", "muat");
    aliasGlobal("loadfile", "muat_file");
    aliasGlobal("dofile", "eksekusi_file");
    aliasGlobal("require", "butuh");
    aliasGlobal("require", "perlu");
}

void LuaiRuntime::registerModuleAliases() {
    auto copyAndEnhance = [this](const char* srcMod, const char* dstMod, const std::vector<std::pair<const char*, const char*>>& aliases) {
        lua_getglobal(L, srcMod);
        if (lua_istable(L, -1)) {
            lua_newtable(L);
            lua_pushnil(L);
            while (lua_next(L, -3) != 0) {
                lua_pushvalue(L, -2);
                lua_insert(L, -2);
                lua_settable(L, -4);
            }
            for (const auto& p : aliases) {
                lua_getfield(L, -2, p.second);
                if (!lua_isnil(L, -1)) {
                    lua_setfield(L, -2, p.first);
                } else {
                    lua_pop(L, 1);
                }
            }
            lua_setglobal(L, dstMod);
        }
        lua_pop(L, 1);
    };

    copyAndEnhance("math", "matematika", {
        {"akar", "sqrt"},
        {"mutlak", "abs"},
        {"lantai", "floor"},
        {"atap", "ceil"},
        {"acak", "random"},
        {"benih_acak", "randomseed"},
        {"minimum", "min"},
        {"maksimum", "max"},
        {"derajat", "deg"},
        {"radian", "rad"}
    });

    copyAndEnhance("table", "tabel", {
        {"sisip", "insert"},
        {"hapus", "remove"},
        {"urut", "sort"},
        {"gabung", "concat"},
        {"pindah", "move"},
        {"bungkus", "pack"},
        {"lepas", "unpack"}
    });

    copyAndEnhance("string", "teks", {
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
    });

    lua_getglobal(L, "string");
    if (lua_istable(L, -1)) {
        std::vector<std::pair<const char*, const char*>> strAliases = {
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
        for (const auto& p : strAliases) {
            lua_getfield(L, -1, p.second);
            if (!lua_isnil(L, -1)) {
                lua_setfield(L, -2, p.first);
            } else {
                lua_pop(L, 1);
            }
        }
    }
    lua_pop(L, 1);

    copyAndEnhance("os", "sistem", {
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
    });

    lua_getglobal(L, "io");
    if (lua_istable(L, -1)) {
        std::vector<std::pair<const char*, const char*>> ioAliases = {
            {"tulis", "write"},
            {"baca", "read"},
            {"buka", "open"},
            {"tutup", "close"},
            {"siram", "flush"},
            {"baris", "lines"},
            {"masukan", "input"},
            {"keluaran", "output"},
            {"tipe", "type"}
        };
        for (const auto& p : ioAliases) {
            lua_getfield(L, -1, p.second);
            if (!lua_isnil(L, -1)) {
                lua_setfield(L, -2, p.first);
            } else {
                lua_pop(L, 1);
            }
        }
    }
    lua_pop(L, 1);

    copyAndEnhance("coroutine", "korutin", {
        {"buat", "create"},
        {"lanjut", "resume"},
        {"hasil", "yield"},
        {"status", "status"},
        {"bungkus", "wrap"},
        {"berjalan", "running"},
        {"bisa_hasil", "isyieldable"},
        {"tutup", "close"}
    });
}

int LuaiRuntime::luaiSearcher(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    std::string modname(name);
    for (char& c : modname) {
        if (c == '.') c = '/';
    }

    std::vector<std::string> searchPaths = {
        modname + ".luai",
        "./" + modname + ".luai",
        "./modules/" + modname + ".luai",
        "./modul/" + modname + ".luai"
    };

    std::string foundPath;
    for (const auto& path : searchPaths) {
        std::ifstream test(path);
        if (test.good()) {
            foundPath = path;
            break;
        }
    }

    if (foundPath.empty()) {
        std::string err = "\n\ttidak ada file luai '" + modname + ".luai'";
        lua_pushstring(L, err.c_str());
        return 1;
    }

    std::ifstream file(foundPath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::string err = "\n\tgagal membuka file luai '" + foundPath + "'";
        lua_pushstring(L, err.c_str());
        return 1;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string transpiled = Lexer::transpile(content);

    std::string chunkName = "@" + foundPath;
    if (luaL_loadbuffer(L, transpiled.data(), transpiled.size(), chunkName.c_str()) != LUA_OK) {
        return lua_error(L);
    }

    lua_pushstring(L, foundPath.c_str());
    return 2;
}

void LuaiRuntime::registerSearcher() {
    lua_getglobal(L, "package");
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "searchers");
        if (lua_istable(L, -1)) {
            int len = (int)lua_rawlen(L, -1);
            for (int i = len; i >= 2; i--) {
                lua_rawgeti(L, -1, i);
                lua_rawseti(L, -2, i + 1);
            }
            lua_pushcfunction(L, luaiSearcher);
            lua_rawseti(L, -2, 2);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

bool LuaiRuntime::executeString(const std::string& code, const std::string& chunkName) {
    lastError.clear();
    std::string transpiled = Lexer::transpile(code);
    std::string chunk = "=" + chunkName;

    if (luaL_loadbuffer(L, transpiled.data(), transpiled.size(), chunk.c_str()) != LUA_OK) {
        lastError = lua_tostring(L, -1);
        lua_pop(L, 1);
        return false;
    }

    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
        lastError = lua_tostring(L, -1);
        lua_pop(L, 1);
        return false;
    }

    return true;
}

bool LuaiRuntime::executeFile(const std::string& filepath, const std::vector<std::string>& args) {
    lastError.clear();
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        lastError = "Tidak dapat membuka file: " + filepath;
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string transpiled = Lexer::transpile(content);

    lua_newtable(L);
    lua_pushstring(L, filepath.c_str());
    lua_rawseti(L, -2, 0);
    for (size_t i = 0; i < args.size(); i++) {
        lua_pushstring(L, args[i].c_str());
        lua_rawseti(L, -2, static_cast<lua_Integer>(i + 1));
    }
    lua_setglobal(L, "arg");

    std::string chunkName = "@" + filepath;
    if (luaL_loadbuffer(L, transpiled.data(), transpiled.size(), chunkName.c_str()) != LUA_OK) {
        lastError = lua_tostring(L, -1);
        lua_pop(L, 1);
        return false;
    }

    for (size_t i = 0; i < args.size(); i++) {
        lua_pushstring(L, args[i].c_str());
    }

    if (lua_pcall(L, static_cast<int>(args.size()), LUA_MULTRET, 0) != LUA_OK) {
        lastError = lua_tostring(L, -1);
        lua_pop(L, 1);
        return false;
    }

    return true;
}

bool LuaiRuntime::evaluateExpression(const std::string& expr, std::string& output) {
    lastError.clear();
    output.clear();

    std::string transpiled = Lexer::transpile(expr);
    std::string returnCode = "return " + transpiled;

    int loadStatus = luaL_loadbuffer(L, returnCode.data(), returnCode.size(), "=luai");
    if (loadStatus != LUA_OK) {
        lua_pop(L, 1);
        loadStatus = luaL_loadbuffer(L, transpiled.data(), transpiled.size(), "=luai");
    }

    if (loadStatus != LUA_OK) {
        lastError = lua_tostring(L, -1);
        lua_pop(L, 1);
        return false;
    }

    int base = lua_gettop(L) - 1;
    int pcallStatus = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (pcallStatus != LUA_OK) {
        lastError = lua_tostring(L, -1);
        lua_pop(L, 1);
        return false;
    }

    int top = lua_gettop(L);
    int nres = top - base;
    for (int i = 1; i <= nres; i++) {
        if (i > 1) output += "\t";
        int idx = base + i;
        if (lua_isboolean(L, idx)) {
            output += (lua_toboolean(L, idx) ? "benar" : "salah");
        } else if (lua_isnil(L, idx)) {
            output += "nihil";
        } else {
            const char* s = luaL_tolstring(L, idx, nullptr);
            output += (s ? s : "");
            lua_pop(L, 1);
        }
    }
    lua_settop(L, base);
    return true;
}

lua_State* LuaiRuntime::getState() const {
    return L;
}

std::string LuaiRuntime::getLastError() const {
    return lastError;
}
