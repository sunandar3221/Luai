#include "json_module.hpp"

#if defined(USE_LUA54)
#include "lua.hpp"
#elif __has_include("../luajit/src/lua.hpp")
#include "../luajit/src/lua.hpp"
#elif __has_include("luajit.h")
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#elif __has_include("lua.hpp")
#include "lua.hpp"
#else
#include "../lua-5.4.7/src/lua.hpp"
#endif

#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <string.h>
#include <cstdint>
#include <algorithm>

#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 502
#ifndef lua_rawlen
#define lua_rawlen(L, i) lua_objlen(L, (i))
#endif
#ifndef lua_absindex
#define lua_absindex(L, i) ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)
#endif
#endif

namespace {

// Penanda null khusus
static const char* JSON_NULL_NAME = "luai.json.null";

// ---------------------------------------------------------------------------
// JSON ENCODER
// ---------------------------------------------------------------------------

struct EncodeContext {
    std::ostringstream ss;
    bool pretty;
    int indentStep;
    int currentIndent;
    std::vector<const void*> visited;

    EncodeContext(bool p, int step)
        : pretty(p), indentStep(step), currentIndent(0) {}

    void writeIndent() {
        if (pretty) {
            ss << "\n";
            for (int i = 0; i < currentIndent; i++) {
                ss << " ";
            }
        }
    }
};

static void escapeJsonString(const char* str, size_t len, std::ostringstream& ss) {
    ss << '"';
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)str[i];
        switch (c) {
            case '"':  ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b";  break;
            case '\f': ss << "\\f";  break;
            case '\n': ss << "\\n";  break;
            case '\r': ss << "\\r";  break;
            case '\t': ss << "\\t";  break;
            default:
                if (c < 32) {
                    char hex[8];
                    snprintf(hex, sizeof(hex), "\\u%04x", c);
                    ss << hex;
                } else {
                    ss << (char)c;
                }
                break;
        }
    }
    ss << '"';
}

static bool isTableArray(lua_State* L, int idx, size_t& arrayLen) {
    idx = lua_absindex(L, idx);
    size_t len = lua_rawlen(L, idx);
    arrayLen = len;

    // Cek apakah ada kunci non-integer
    size_t count = 0;
    lua_pushnil(L);
    while (lua_next(L, idx) != 0) {
        count++;
        if (lua_type(L, -2) != LUA_TNUMBER) {
            lua_pop(L, 2);
            return false;
        }
        lua_Number num = lua_tonumber(L, -2);
        if (std::floor(num) != num || num < 1 || (size_t)num > len) {
            lua_pop(L, 2);
            return false;
        }
        lua_pop(L, 1);
    }

    if (count == 0) {
        // Tabel kosong: bisa diperlakukan sebagai object {}
        return false;
    }

    return count == len;
}

static void encodeValue(lua_State* L, int idx, EncodeContext& ctx) {
    idx = lua_absindex(L, idx);
    int t = lua_type(L, idx);

    if (t == LUA_TNIL) {
        ctx.ss << "null";
    } else if (t == LUA_TBOOLEAN) {
        ctx.ss << (lua_toboolean(L, idx) ? "true" : "false");
    } else if (t == LUA_TNUMBER) {
        lua_Number n = lua_tonumber(L, idx);
        if (std::isnan(n) || std::isinf(n)) {
            ctx.ss << "null";
        } else {
            double intpart;
            if (std::modf(n, &intpart) == 0.0 && n >= -9007199254740992.0 && n <= 9007199254740992.0) {
                ctx.ss << (long long)n;
            } else {
                char buf[64];
                snprintf(buf, sizeof(buf), "%.14g", n);
                ctx.ss << buf;
            }
        }
    } else if (t == LUA_TSTRING) {
        size_t len = 0;
        const char* s = lua_tolstring(L, idx, &len);
        escapeJsonString(s, len, ctx.ss);
    } else if (t == LUA_TLIGHTUSERDATA || t == LUA_TUSERDATA) {
        // Cek apakah ini json.null
        if (lua_touserdata(L, idx) == (void*)JSON_NULL_NAME) {
            ctx.ss << "null";
        } else {
            ctx.ss << "null";
        }
    } else if (t == LUA_TTABLE) {
        // Cek circular reference
        const void* ptr = lua_topointer(L, idx);
        for (const void* p : ctx.visited) {
            if (p == ptr) {
                luaL_error(L, "json.kodekan gagal: terdeteksi referensi melingkar (circular reference) pada tabel");
                return;
            }
        }
        ctx.visited.push_back(ptr);

        size_t arrLen = 0;
        bool isArray = isTableArray(L, idx, arrLen);

        if (isArray) {
            if (arrLen == 0) {
                ctx.ss << "[]";
            } else {
                ctx.ss << "[";
                ctx.currentIndent += ctx.indentStep;
                for (size_t i = 1; i <= arrLen; i++) {
                    if (i > 1) ctx.ss << ",";
                    ctx.writeIndent();
                    lua_rawgeti(L, idx, (lua_Integer)i);
                    encodeValue(L, -1, ctx);
                    lua_pop(L, 1);
                }
                ctx.currentIndent -= ctx.indentStep;
                ctx.writeIndent();
                ctx.ss << "]";
            }
        } else {
            // Object: urutkan atau iterasi pasangan kunci-nilai
            ctx.ss << "{";
            ctx.currentIndent += ctx.indentStep;
            bool first = true;
            lua_pushnil(L);
            while (lua_next(L, idx) != 0) {
                if (!first) ctx.ss << ",";
                first = false;
                ctx.writeIndent();

                // Format kunci
                size_t klen = 0;
                const char* ks = lua_tolstring(L, -2, &klen);
                if (ks) {
                    escapeJsonString(ks, klen, ctx.ss);
                } else {
                    escapeJsonString("", 0, ctx.ss);
                }

                ctx.ss << (ctx.pretty ? ": " : ":");
                encodeValue(L, -1, ctx);
                lua_pop(L, 1);
            }
            ctx.currentIndent -= ctx.indentStep;
            if (!first) {
                ctx.writeIndent();
            }
            ctx.ss << "}";
        }

        ctx.visited.pop_back();
    } else {
        ctx.ss << "null";
    }
}

// ---------------------------------------------------------------------------
// JSON DECODER
// ---------------------------------------------------------------------------

struct DecodeContext {
    const char* src;
    size_t len;
    size_t pos;
    int line;
    int col;

    DecodeContext(const char* s, size_t l)
        : src(s), len(l), pos(0), line(1), col(1) {}

    char peek() const {
        return (pos < len) ? src[pos] : '\0';
    }

    char next() {
        if (pos >= len) return '\0';
        char c = src[pos++];
        if (c == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }
        return c;
    }

    void skipWhitespace() {
        while (pos < len) {
            char c = src[pos];
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                next();
            } else {
                break;
            }
        }
    }

    std::string errorMsg(const std::string& msg) const {
        std::ostringstream err;
        err << "Kesalahan JSON pada baris " << line << ", kolom " << col << ": " << msg;
        return err.str();
    }
};

static bool parseValue(lua_State* L, DecodeContext& ctx, std::string& err);

static bool parseString(DecodeContext& ctx, std::string& out, std::string& err) {
    ctx.next(); // lewati kutip pembuka '"'
    out.clear();

    while (ctx.pos < ctx.len) {
        char c = ctx.next();
        if (c == '"') {
            return true;
        }
        if (c == '\\') {
            if (ctx.pos >= ctx.len) {
                err = ctx.errorMsg("Karakter escape belum selesai pada akhir teks");
                return false;
            }
            char esc = ctx.next();
            switch (esc) {
                case '"':  out.push_back('"'); break;
                case '\\': out.push_back('\\'); break;
                case '/':  out.push_back('/'); break;
                case 'b':  out.push_back('\b'); break;
                case 'f':  out.push_back('\f'); break;
                case 'n':  out.push_back('\n'); break;
                case 'r':  out.push_back('\r'); break;
                case 't':  out.push_back('\t'); break;
                case 'u': {
                    // 4 digit hexadecimal unicode
                    if (ctx.pos + 4 > ctx.len) {
                        err = ctx.errorMsg("Unicode escape \\uXXXX tidak lengkap");
                        return false;
                    }
                    uint32_t codepoint = 0;
                    for (int i = 0; i < 4; i++) {
                        char h = ctx.next();
                        codepoint <<= 4;
                        if (h >= '0' && h <= '9') codepoint |= (h - '0');
                        else if (h >= 'a' && h <= 'f') codepoint |= (h - 'a' + 10);
                        else if (h >= 'A' && h <= 'F') codepoint |= (h - 'A' + 10);
                        else {
                            err = ctx.errorMsg("Digit hex tidak valid pada \\uXXXX");
                            return false;
                        }
                    }
                    // Konversi codepoint ke UTF-8
                    if (codepoint <= 0x7F) {
                        out.push_back((char)codepoint);
                    } else if (codepoint <= 0x7FF) {
                        out.push_back((char)(0xC0 | (codepoint >> 6)));
                        out.push_back((char)(0x80 | (codepoint & 0x3F)));
                    } else {
                        out.push_back((char)(0xE0 | (codepoint >> 12)));
                        out.push_back((char)(0x80 | ((codepoint >> 6) & 0x3F)));
                        out.push_back((char)(0x80 | (codepoint & 0x3F)));
                    }
                    break;
                }
                default:
                    err = ctx.errorMsg(std::string("Karakter escape tidak valid '\\") + esc + "'");
                    return false;
            }
        } else {
            out.push_back(c);
        }
    }

    err = ctx.errorMsg("String JSON tidak ditutup dengan tanda kutip ganda (\")");
    return false;
}

static bool parseNumber(DecodeContext& ctx, double& outNum, bool& isInteger, long long& outInt, std::string& err) {
    size_t start = ctx.pos;
    if (ctx.peek() == '-') {
        ctx.next();
    }

    if (ctx.peek() == '0') {
        ctx.next();
    } else if (std::isdigit((unsigned char)ctx.peek())) {
        while (std::isdigit((unsigned char)ctx.peek())) {
            ctx.next();
        }
    } else {
        err = ctx.errorMsg("Format angka JSON tidak valid");
        return false;
    }

    bool hasFraction = false;
    if (ctx.peek() == '.') {
        hasFraction = true;
        ctx.next();
        if (!std::isdigit((unsigned char)ctx.peek())) {
            err = ctx.errorMsg("Angka desimal memerlukan digit setelah tanda titik");
            return false;
        }
        while (std::isdigit((unsigned char)ctx.peek())) {
            ctx.next();
        }
    }

    bool hasExp = false;
    if (ctx.peek() == 'e' || ctx.peek() == 'E') {
        hasExp = true;
        ctx.next();
        if (ctx.peek() == '+' || ctx.peek() == '-') {
            ctx.next();
        }
        if (!std::isdigit((unsigned char)ctx.peek())) {
            err = ctx.errorMsg("Eksponen angka memerlukan digit setelah tanda e/E");
            return false;
        }
        while (std::isdigit((unsigned char)ctx.peek())) {
            ctx.next();
        }
    }

    std::string numStr(ctx.src + start, ctx.pos - start);
    char* endptr = nullptr;
    outNum = std::strtod(numStr.c_str(), &endptr);
    if (endptr != numStr.c_str() + numStr.size()) {
        err = ctx.errorMsg("Gagal mengurai angka: " + numStr);
        return false;
    }

    if (!hasFraction && !hasExp) {
        isInteger = true;
        outInt = std::strtoll(numStr.c_str(), nullptr, 10);
    } else {
        isInteger = false;
    }

    return true;
}

static bool parseArray(lua_State* L, DecodeContext& ctx, std::string& err) {
    ctx.next(); // lewati '['
    ctx.skipWhitespace();

    lua_newtable(L);
    int arrIdx = 1;

    if (ctx.peek() == ']') {
        ctx.next();
        return true;
    }

    while (true) {
        ctx.skipWhitespace();
        if (!parseValue(L, ctx, err)) {
            return false;
        }
        lua_rawseti(L, -2, arrIdx++);

        ctx.skipWhitespace();
        char c = ctx.peek();
        if (c == ',') {
            ctx.next();
        } else if (c == ']') {
            ctx.next();
            break;
        } else {
            err = ctx.errorMsg("Diharapkan tanda koma (,) atau tanda kurung siku penutup (]) pada larik JSON");
            return false;
        }
    }

    return true;
}

static bool parseObject(lua_State* L, DecodeContext& ctx, std::string& err) {
    ctx.next(); // lewati '{'
    ctx.skipWhitespace();

    lua_newtable(L);

    if (ctx.peek() == '}') {
        ctx.next();
        return true;
    }

    while (true) {
        ctx.skipWhitespace();
        if (ctx.peek() != '"') {
            err = ctx.errorMsg("Kunci objek JSON harus berupa teks string dengan tanda kutip ganda (\")");
            return false;
        }

        std::string key;
        if (!parseString(ctx, key, err)) {
            return false;
        }

        ctx.skipWhitespace();
        if (ctx.peek() != ':') {
            err = ctx.errorMsg("Diharapkan tanda titik dua (:) setelah kunci objek JSON");
            return false;
        }
        ctx.next(); // lewati ':'

        ctx.skipWhitespace();
        if (!parseValue(L, ctx, err)) {
            return false;
        }

        lua_setfield(L, -2, key.c_str());

        ctx.skipWhitespace();
        char c = ctx.peek();
        if (c == ',') {
            ctx.next();
        } else if (c == '}') {
            ctx.next();
            break;
        } else {
            err = ctx.errorMsg("Diharapkan tanda koma (,) atau kurung kurawal penutup (}) pada objek JSON");
            return false;
        }
    }

    return true;
}

static bool parseValue(lua_State* L, DecodeContext& ctx, std::string& err) {
    ctx.skipWhitespace();
    char c = ctx.peek();

    if (c == '"') {
        std::string str;
        if (!parseString(ctx, str, err)) return false;
        lua_pushlstring(L, str.data(), str.size());
        return true;
    }

    if (c == '{') {
        return parseObject(L, ctx, err);
    }

    if (c == '[') {
        return parseArray(L, ctx, err);
    }

    if (c == '-' || std::isdigit((unsigned char)c)) {
        double d = 0;
        bool isInt = false;
        long long i = 0;
        if (!parseNumber(ctx, d, isInt, i, err)) return false;
        if (isInt) {
            lua_pushinteger(L, (lua_Integer)i);
        } else {
            lua_pushnumber(L, (lua_Number)d);
        }
        return true;
    }

    // Literals: true, false, null
    if (ctx.pos + 4 <= ctx.len && memcmp(ctx.src + ctx.pos, "true", 4) == 0) {
        ctx.pos += 4; ctx.col += 4;
        lua_pushboolean(L, 1);
        return true;
    }
    if (ctx.pos + 5 <= ctx.len && memcmp(ctx.src + ctx.pos, "false", 5) == 0) {
        ctx.pos += 5; ctx.col += 5;
        lua_pushboolean(L, 0);
        return true;
    }
    if (ctx.pos + 4 <= ctx.len && memcmp(ctx.src + ctx.pos, "null", 4) == 0) {
        ctx.pos += 4; ctx.col += 4;
        // Dorong json.null
        lua_getglobal(L, "json");
        if (lua_istable(L, -1)) {
            lua_getfield(L, -1, "null");
            lua_remove(L, -2);
        } else {
            lua_pop(L, 1);
            lua_pushnil(L);
        }
        return true;
    }

    if (c == '\0') {
        err = ctx.errorMsg("Tiba di akhir teks saat sedang mengurai nilai JSON");
    } else {
        err = ctx.errorMsg(std::string("Karakter tak terduga dalam JSON: '") + c + "'");
    }
    return false;
}

// ---------------------------------------------------------------------------
// LUA BINDINGS
// ---------------------------------------------------------------------------

// json.encode(val, [pretty])
// json.kodekan(val, [rapi])
static int luai_json_encode(lua_State* L) {
    luaL_checkany(L, 1);

    bool pretty = false;
    int step = 2;

    if (lua_gettop(L) >= 2) {
        if (lua_isboolean(L, 2)) {
            pretty = lua_toboolean(L, 2) != 0;
        } else if (lua_isnumber(L, 2)) {
            pretty = true;
            step = (int)lua_tointeger(L, 2);
            if (step < 1) step = 1;
            if (step > 8) step = 8;
        }
    }

    EncodeContext ctx(pretty, step);
    encodeValue(L, 1, ctx);

    std::string result = ctx.ss.str();
    lua_pushlstring(L, result.data(), result.size());
    return 1;
}

// json.decode(str)
// json.uraikan(str)
static int luai_json_decode(lua_State* L) {
    size_t len = 0;
    const char* str = luaL_checklstring(L, 1, &len);

    DecodeContext ctx(str, len);
    std::string err;
    if (!parseValue(L, ctx, err)) {
        return luaL_error(L, "%s", err.c_str());
    }

    ctx.skipWhitespace();
    if (ctx.pos < ctx.len) {
        std::string trailingErr = ctx.errorMsg("Ditemukan teks tambahan tak terduga setelah JSON berakhir");
        return luaL_error(L, "%s", trailingErr.c_str());
    }

    return 1;
}

// json.decode_safe(str)
// json.uraikan_aman(str)
static int luai_json_decode_safe(lua_State* L) {
    size_t len = 0;
    const char* str = luaL_checklstring(L, 1, &len);

    DecodeContext ctx(str, len);
    std::string err;
    if (!parseValue(L, ctx, err)) {
        lua_pushnil(L);
        lua_pushlstring(L, err.data(), err.size());
        return 2;
    }

    ctx.skipWhitespace();
    if (ctx.pos < ctx.len) {
        std::string trailingErr = ctx.errorMsg("Ditemukan teks tambahan tak terduga setelah JSON berakhir");
        lua_pop(L, 1);
        lua_pushnil(L);
        lua_pushlstring(L, trailingErr.data(), trailingErr.size());
        return 2;
    }

    return 1;
}

} // namespace

void luai_register_json_module(lua_State* L) {
    lua_newtable(L);

    // Fungsi utama
    lua_pushcfunction(L, luai_json_encode);
    lua_setfield(L, -2, "encode");

    lua_pushcfunction(L, luai_json_encode);
    lua_setfield(L, -2, "kodekan");

    lua_pushcfunction(L, luai_json_encode);
    lua_setfield(L, -2, "tulis");

    lua_pushcfunction(L, luai_json_encode);
    lua_setfield(L, -2, "ke_json");

    lua_pushcfunction(L, luai_json_decode);
    lua_setfield(L, -2, "decode");

    lua_pushcfunction(L, luai_json_decode);
    lua_setfield(L, -2, "uraikan");

    lua_pushcfunction(L, luai_json_decode);
    lua_setfield(L, -2, "baca");

    lua_pushcfunction(L, luai_json_decode);
    lua_setfield(L, -2, "dari_json");

    lua_pushcfunction(L, luai_json_decode_safe);
    lua_setfield(L, -2, "decode_safe");

    lua_pushcfunction(L, luai_json_decode_safe);
    lua_setfield(L, -2, "uraikan_aman");

    // json.null / json.nihil penanda null
    lua_pushlightuserdata(L, (void*)JSON_NULL_NAME);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, "null");
    lua_setfield(L, -2, "nihil");

    // Simpan ke global 'json'
    lua_pushvalue(L, -1);
    lua_setglobal(L, "json");

    // Daftarkan ke package.loaded['json'] agar bisa: lokal json = butuh("json")
    lua_getglobal(L, "package");
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "loaded");
        if (lua_istable(L, -1)) {
            lua_pushvalue(L, -3);
            lua_setfield(L, -2, "json");
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 2); // pop package dan table json
}
