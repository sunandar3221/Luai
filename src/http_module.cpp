#include "http_module.hpp"

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
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cctype>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>
#else
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 502
#ifndef lua_absindex
#define lua_absindex(L, i) ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)
#endif
#endif

namespace {

struct HttpResponse {
    int statusCode = 0;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string error;
    bool success = false;
};

struct HttpRequest {
    std::string url;
    std::string method = "GET";
    std::map<std::string, std::string> headers;
    std::string body;
    int timeoutSeconds = 30;
};

// ---------------------------------------------------------------------------
// URL PARSER SEDERHANA
// ---------------------------------------------------------------------------
struct ParsedUrl {
    bool isHttps = true;
    std::string host;
    int port = 443;
    std::string path = "/";
};

static bool parseUrl(const std::string& url, ParsedUrl& out, std::string& err) {
    std::string s = url;
    size_t schemePos = s.find("://");
    if (schemePos == std::string::npos) {
        // Asumsikan https:// jika tidak disertakan
        out.isHttps = true;
        out.port = 443;
    } else {
        std::string scheme = s.substr(0, schemePos);
        std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);
        if (scheme == "https") {
            out.isHttps = true;
            out.port = 443;
        } else if (scheme == "http") {
            out.isHttps = false;
            out.port = 80;
        } else {
            err = "Protokol URL tidak didukung (hanya 'http' dan 'https'): " + scheme;
            return false;
        }
        s = s.substr(schemePos + 3);
    }

    size_t pathPos = s.find('/');
    std::string hostPart;
    if (pathPos == std::string::npos) {
        hostPart = s;
        out.path = "/";
    } else {
        hostPart = s.substr(0, pathPos);
        out.path = s.substr(pathPos);
        if (out.path.empty()) out.path = "/";
    }

    // Cek port custom di host (host:port)
    size_t colonPos = hostPart.find(':');
    if (colonPos != std::string::npos) {
        out.host = hostPart.substr(0, colonPos);
        std::string portStr = hostPart.substr(colonPos + 1);
        try {
            out.port = std::stoi(portStr);
        } catch (...) {
            err = "Format port URL tidak valid: " + portStr;
            return false;
        }
    } else {
        out.host = hostPart;
    }

    if (out.host.empty()) {
        err = "Nama host URL tidak boleh kosong";
        return false;
    }

    return true;
}

#if defined(_WIN32)
// ---------------------------------------------------------------------------
// IMPLEMENTASI WINDOWS (WINHTTP NATIVE)
// ---------------------------------------------------------------------------
static std::wstring utf8ToWide(const std::string& str) {
    if (str.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &result[0], size);
    return result;
}

static std::string wideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &result[0], size, nullptr, nullptr);
    return result;
}

static HttpResponse executeRequestWindows(const HttpRequest& req) {
    HttpResponse resp;

    ParsedUrl purl;
    std::string parseErr;
    if (!parseUrl(req.url, purl, parseErr)) {
        resp.error = parseErr;
        return resp;
    }

    HINTERNET hSession = WinHttpOpen(
        L"Luai-Http/1.0 (Windows; Dialek Bahasa Indonesia)",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );

    if (!hSession) {
        resp.error = "Gagal menginisialisasi sesi WinHTTP (kode: " + std::to_string(GetLastError()) + ")";
        return resp;
    }

    // Set timeout
    int timeoutMs = req.timeoutSeconds * 1000;
    WinHttpSetTimeouts(hSession, timeoutMs, timeoutMs, timeoutMs, timeoutMs);

    std::wstring hostW = utf8ToWide(purl.host);
    HINTERNET hConnect = WinHttpConnect(hSession, hostW.c_str(), (INTERNET_PORT)purl.port, 0);
    if (!hConnect) {
        resp.error = "Gagal terhubung ke host '" + purl.host + "' (kode: " + std::to_string(GetLastError()) + ")";
        WinHttpCloseHandle(hSession);
        return resp;
    }

    std::wstring methodW = utf8ToWide(req.method);
    std::wstring pathW = utf8ToWide(purl.path);
    DWORD reqFlags = purl.isHttps ? WINHTTP_FLAG_SECURE : 0;

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        methodW.c_str(),
        pathW.c_str(),
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        reqFlags
    );

    if (!hRequest) {
        resp.error = "Gagal membuka permintaan HTTP (kode: " + std::to_string(GetLastError()) + ")";
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return resp;
    }

    // Ikuti redirect secara otomatis
    DWORD redirectPolicy = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_REDIRECT_POLICY, &redirectPolicy, sizeof(redirectPolicy));

    // Tambahkan header
    std::wstring allHeadersW;
    for (const auto& h : req.headers) {
        std::wstring hw = utf8ToWide(h.first + ": " + h.second + "\r\n");
        allHeadersW += hw;
    }

    if (!allHeadersW.empty()) {
        WinHttpAddRequestHeaders(
            hRequest,
            allHeadersW.c_str(),
            (DWORD)allHeadersW.length(),
            WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE
        );
    }

    // Kirim request
    LPVOID bodyPtr = (req.body.empty()) ? WINHTTP_NO_REQUEST_DATA : (LPVOID)req.body.data();
    DWORD bodyLen = (DWORD)req.body.size();

    BOOL bSend = WinHttpSendRequest(
        hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        bodyPtr,
        bodyLen,
        bodyLen,
        0
    );

    if (!bSend) {
        resp.error = "Gagal mengirim permintaan HTTP ke '" + req.url + "' (kode: " + std::to_string(GetLastError()) + ")";
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return resp;
    }

    if (!WinHttpReceiveResponse(hRequest, nullptr)) {
        resp.error = "Gagal menerima respons HTTP dari '" + req.url + "' (kode: " + std::to_string(GetLastError()) + ")";
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return resp;
    }

    // Baca status code
    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize, WINHTTP_NO_HEADER_INDEX)) {
        resp.statusCode = (int)statusCode;
    }

    // Baca response headers
    DWORD headerBufSize = 0;
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, nullptr, &headerBufSize, WINHTTP_NO_HEADER_INDEX);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER && headerBufSize > 0) {
        std::vector<wchar_t> headerBuf(headerBufSize / sizeof(wchar_t) + 1, 0);
        if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, headerBuf.data(), &headerBufSize, WINHTTP_NO_HEADER_INDEX)) {
            std::string rawHeaders = wideToUtf8(headerBuf.data());
            std::istringstream hss(rawHeaders);
            std::string hline;
            while (std::getline(hss, hline)) {
                if (!hline.empty() && hline.back() == '\r') hline.pop_back();
                size_t cpos = hline.find(':');
                if (cpos != std::string::npos) {
                    std::string k = hline.substr(0, cpos);
                    std::string v = hline.substr(cpos + 1);
                    while (!v.empty() && (v.front() == ' ' || v.front() == '\t')) v.erase(v.begin());
                    resp.headers[k] = v;
                }
            }
        }
    }

    // Baca response body
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    std::string responseBody;

    do {
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
            break;
        }
        if (dwSize == 0) {
            break;
        }

        std::vector<char> buffer(dwSize + 1, 0);
        if (WinHttpReadData(hRequest, (LPVOID)buffer.data(), dwSize, &dwDownloaded)) {
            responseBody.append(buffer.data(), dwDownloaded);
        } else {
            break;
        }
    } while (dwSize > 0);

    resp.body = responseBody;
    resp.success = (resp.statusCode >= 200 && resp.statusCode < 400);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return resp;
}

#else
// ---------------------------------------------------------------------------
// IMPLEMENTASI LINUX / ANDROID TERMUX (MENGGUNAKAN CURL BAWAAN)
// ---------------------------------------------------------------------------
static HttpResponse executeRequestCurl(const HttpRequest& req) {
    HttpResponse resp;

    // Siapkan berkas payload jika ada body
    char bodyTempPath[256] = {0};
    bool hasTempBody = false;

    if (!req.body.empty()) {
        const char* tmpDir = getenv("TMPDIR");
        if (!tmpDir || tmpDir[0] == '\0') tmpDir = "/tmp";
        snprintf(bodyTempPath, sizeof(bodyTempPath), "%s/luai_http_%d_%ld.tmp", tmpDir, (int)getpid(), (long)time(nullptr));
        std::ofstream bfile(bodyTempPath, std::ios::out | std::ios::binary);
        if (bfile.is_open()) {
            bfile.write(req.body.data(), req.body.size());
            bfile.close();
            hasTempBody = true;
        }
    }

    // Bangun perintah curl
    std::ostringstream cmd;
    cmd << "curl -s -S -i -L --max-time " << req.timeoutSeconds;
    cmd << " -X \"" << req.method << "\"";

    for (const auto& h : req.headers) {
        std::string escapedVal = h.second;
        // Ganti kutip ganda
        size_t q = 0;
        while ((q = escapedVal.find('"', q)) != std::string::npos) {
            escapedVal.replace(q, 1, "\\\"");
            q += 2;
        }
        cmd << " -H \"" << h.first << ": " << escapedVal << "\"";
    }

    if (hasTempBody) {
        cmd << " --data-binary \"@" << bodyTempPath << "\"";
    }

    cmd << " \"" << req.url << "\" 2>&1";

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) {
        if (hasTempBody) unlink(bodyTempPath);
        resp.error = "Gagal menjalankan utilitas 'curl' pada sistem";
        return resp;
    }

    std::string rawOutput;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        rawOutput += buffer;
    }
    int status = pclose(pipe);

    if (hasTempBody) {
        unlink(bodyTempPath);
    }

    if (status != 0 && rawOutput.find("HTTP/") == std::string::npos) {
        resp.error = "Permintaan HTTP gagal: " + rawOutput;
        return resp;
    }

    // Parse status line, headers, dan body dari rawOutput (bisa ada multiple header block akibat redirect -L)
    size_t lastHeaderEnd = 0;
    size_t searchPos = 0;

    while (true) {
        size_t nextDoubleNewline = rawOutput.find("\r\n\r\n", searchPos);
        size_t sepLen = 4;
        if (nextDoubleNewline == std::string::npos) {
            nextDoubleNewline = rawOutput.find("\n\n", searchPos);
            sepLen = 2;
        }

        if (nextDoubleNewline != std::string::npos) {
            std::string chunk = rawOutput.substr(searchPos, nextDoubleNewline - searchPos);
            if (chunk.find("HTTP/") == 0 || chunk.find("HTTP/") != std::string::npos) {
                lastHeaderEnd = nextDoubleNewline + sepLen;
                searchPos = lastHeaderEnd;
            } else {
                break;
            }
        } else {
            break;
        }
    }

    std::string headersSection;
    if (lastHeaderEnd > 0) {
        // Ambil header block terakhir
        size_t prevHeaderStart = rawOutput.rfind("HTTP/", lastHeaderEnd - 4);
        if (prevHeaderStart == std::string::npos) prevHeaderStart = 0;
        headersSection = rawOutput.substr(prevHeaderStart, lastHeaderEnd - prevHeaderStart);
        resp.body = rawOutput.substr(lastHeaderEnd);
    } else {
        resp.body = rawOutput;
    }

    // Parse status code & headers
    std::istringstream hss(headersSection);
    std::string hline;
    bool statusParsed = false;

    while (std::getline(hss, hline)) {
        if (!hline.empty() && hline.back() == '\r') hline.pop_back();
        if (hline.empty()) continue;

        if (!statusParsed && hline.find("HTTP/") == 0) {
            std::istringstream lineStream(hline);
            std::string proto;
            int code = 0;
            lineStream >> proto >> code;
            resp.statusCode = code;
            statusParsed = true;
        } else {
            size_t cpos = hline.find(':');
            if (cpos != std::string::npos) {
                std::string k = hline.substr(0, cpos);
                std::string v = hline.substr(cpos + 1);
                while (!v.empty() && (v.front() == ' ' || v.front() == '\t')) v.erase(v.begin());
                resp.headers[k] = v;
            }
        }
    }

    resp.success = (resp.statusCode >= 200 && resp.statusCode < 400);
    return resp;
}
#endif

// ---------------------------------------------------------------------------
// EKSEKUSI REQUEST UNIFIED
// ---------------------------------------------------------------------------
static HttpResponse executeRequest(const HttpRequest& req) {
#if defined(_WIN32)
    return executeRequestWindows(req);
#else
    return executeRequestCurl(req);
#endif
}

// ---------------------------------------------------------------------------
// RESPONSE TO LUA TABLE
// ---------------------------------------------------------------------------
static int luai_response_json(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getfield(L, 1, "tubuh");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_getfield(L, 1, "body");
    }

    if (!lua_isstring(L, -1)) {
        lua_pop(L, 1);
        return luaL_error(L, "respons:json() gagal: isi tubuh respons bukan teks string");
    }

    // Ambil json.decode
    lua_getglobal(L, "json");
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "decode");
        lua_pushvalue(L, -3); // argumen teks string tubuh
        if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
            return lua_error(L);
        }
        return 1;
    }
    return luaL_error(L, "respons:json() gagal: modul 'json' tidak ditemukan");
}

static void pushHttpResponseTable(lua_State* L, const HttpResponse& resp) {
    lua_newtable(L);

    // status (number)
    lua_pushinteger(L, resp.statusCode);
    lua_setfield(L, -2, "status");

    // sukses / success (boolean)
    lua_pushboolean(L, resp.success ? 1 : 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, "sukses");
    lua_setfield(L, -2, "success");

    // tubuh / body (string)
    lua_pushlstring(L, resp.body.data(), resp.body.size());
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, "tubuh");
    lua_setfield(L, -2, "body");

    // kesalahan / error (string or nil)
    if (!resp.error.empty()) {
        lua_pushlstring(L, resp.error.data(), resp.error.size());
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, "kesalahan");
        lua_setfield(L, -2, "error");
    } else {
        lua_pushnil(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, "kesalahan");
        lua_setfield(L, -2, "error");
    }

    // kepala / headers (table)
    lua_newtable(L);
    for (const auto& h : resp.headers) {
        lua_pushlstring(L, h.second.data(), h.second.size());
        lua_setfield(L, -2, h.first.c_str());
    }
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, "kepala");
    lua_setfield(L, -2, "headers");

    // method respons:json()
    lua_pushcfunction(L, luai_response_json);
    lua_setfield(L, -2, "json");
}

// ---------------------------------------------------------------------------
// PARSER OPSI REQUEST DARI LUA
// ---------------------------------------------------------------------------
static void parseOptionsTable(lua_State* L, int idx, HttpRequest& req) {
    idx = lua_absindex(L, idx);
    if (!lua_istable(L, idx)) return;

    // url
    lua_getfield(L, idx, "url");
    if (lua_isstring(L, -1)) {
        req.url = lua_tostring(L, -1);
    }
    lua_pop(L, 1);

    // metode / method
    lua_getfield(L, idx, "metode");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_getfield(L, idx, "method");
    }
    if (lua_isstring(L, -1)) {
        req.method = lua_tostring(L, -1);
        std::transform(req.method.begin(), req.method.end(), req.method.begin(), ::toupper);
    }
    lua_pop(L, 1);

    // waktu_tunggu / timeout
    lua_getfield(L, idx, "waktu_tunggu");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_getfield(L, idx, "timeout");
    }
    if (lua_isnumber(L, -1)) {
        req.timeoutSeconds = (int)lua_tointeger(L, -1);
        if (req.timeoutSeconds < 1) req.timeoutSeconds = 1;
    }
    lua_pop(L, 1);

    // kepala / headers
    lua_getfield(L, idx, "kepala");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_getfield(L, idx, "headers");
    }
    if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            if (lua_isstring(L, -2) && lua_isstring(L, -1)) {
                req.headers[lua_tostring(L, -2)] = lua_tostring(L, -1);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    // tubuh / body / data
    lua_getfield(L, idx, "tubuh");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_getfield(L, idx, "body");
    }
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_getfield(L, idx, "data");
    }

    if (lua_isstring(L, -1)) {
        size_t len = 0;
        const char* s = lua_tolstring(L, -1, &len);
        req.body.assign(s, len);
    } else if (lua_istable(L, -1)) {
        // Otomatis json.encode jika data berupa tabel
        lua_getglobal(L, "json");
        if (lua_istable(L, -1)) {
            lua_getfield(L, -1, "encode");
            lua_pushvalue(L, -3); // tabel tubuh
            if (lua_pcall(L, 1, 1, 0) == LUA_OK && lua_isstring(L, -1)) {
                size_t len = 0;
                const char* s = lua_tolstring(L, -1, &len);
                req.body.assign(s, len);
                if (req.headers.find("Content-Type") == req.headers.end()) {
                    req.headers["Content-Type"] = "application/json";
                }
            }
            lua_pop(L, 1); // hasil encode
        }
        lua_pop(L, 1); // json table
    }
    lua_pop(L, 1); // tubuh/body
}

// ---------------------------------------------------------------------------
// LUA API FUNCTIONS
// ---------------------------------------------------------------------------

// https.ambil(url, [opsi]) / https.get(url, [opsi])
static int luai_http_get(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    HttpRequest req;
    req.url = url;
    req.method = "GET";

    if (lua_gettop(L) >= 2 && lua_istable(L, 2)) {
        parseOptionsTable(L, 2, req);
        req.url = url; // pastikan url dari argumen 1
        req.method = "GET";
    }

    HttpResponse resp = executeRequest(req);
    pushHttpResponseTable(L, resp);
    return 1;
}

// https.kirim(url, data_atau_opsi) / https.post(url, data_atau_opsi)
static int luai_http_post(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    HttpRequest req;
    req.url = url;
    req.method = "POST";

    if (lua_gettop(L) >= 2) {
        if (lua_isstring(L, 2)) {
            size_t len = 0;
            const char* s = lua_tolstring(L, 2, &len);
            req.body.assign(s, len);
        } else if (lua_istable(L, 2)) {
            // Cek apakah tabel ini berisi konfigurasi {tubuh = ...} atau data murni
            lua_getfield(L, 2, "tubuh");
            bool hasTubuh = !lua_isnil(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, 2, "body");
            bool hasBody = !lua_isnil(L, -1);
            lua_pop(L, 1);

            if (hasTubuh || hasBody) {
                parseOptionsTable(L, 2, req);
                req.url = url;
                req.method = "POST";
            } else {
                // Konversi tabel langsung ke JSON payload
                lua_getglobal(L, "json");
                if (lua_istable(L, -1)) {
                    lua_getfield(L, -1, "encode");
                    lua_pushvalue(L, 2);
                    if (lua_pcall(L, 1, 1, 0) == LUA_OK && lua_isstring(L, -1)) {
                        size_t len = 0;
                        const char* s = lua_tolstring(L, -1, &len);
                        req.body.assign(s, len);
                        req.headers["Content-Type"] = "application/json";
                    }
                    lua_pop(L, 1);
                }
                lua_pop(L, 1);
            }
        }
    }

    HttpResponse resp = executeRequest(req);
    pushHttpResponseTable(L, resp);
    return 1;
}

// https.taruh(url, data_atau_opsi) / https.put(url, data_atau_opsi)
static int luai_http_put(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    HttpRequest req;
    req.url = url;
    req.method = "PUT";

    if (lua_gettop(L) >= 2) {
        if (lua_isstring(L, 2)) {
            size_t len = 0;
            const char* s = lua_tolstring(L, 2, &len);
            req.body.assign(s, len);
        } else if (lua_istable(L, 2)) {
            parseOptionsTable(L, 2, req);
            req.url = url;
            req.method = "PUT";
        }
    }

    HttpResponse resp = executeRequest(req);
    pushHttpResponseTable(L, resp);
    return 1;
}

// https.hapus(url, [opsi]) / https.delete(url, [opsi])
static int luai_http_delete(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    HttpRequest req;
    req.url = url;
    req.method = "DELETE";

    if (lua_gettop(L) >= 2 && lua_istable(L, 2)) {
        parseOptionsTable(L, 2, req);
        req.url = url;
        req.method = "DELETE";
    }

    HttpResponse resp = executeRequest(req);
    pushHttpResponseTable(L, resp);
    return 1;
}

// https.permintaan(opsi) / https.request(opsi)
static int luai_http_request(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    HttpRequest req;
    parseOptionsTable(L, 1, req);

    if (req.url.empty()) {
        return luaL_error(L, "https.permintaan gagal: parameter 'url' wajib disertakan dalam tabel");
    }

    HttpResponse resp = executeRequest(req);
    pushHttpResponseTable(L, resp);
    return 1;
}

} // namespace

void luai_register_http_module(lua_State* L) {
    auto setupHttpTable = [&](const char* modName) {
        lua_newtable(L);

        // GET
        lua_pushcfunction(L, luai_http_get);
        lua_setfield(L, -2, "ambil");
        lua_pushcfunction(L, luai_http_get);
        lua_setfield(L, -2, "get");

        // POST
        lua_pushcfunction(L, luai_http_post);
        lua_setfield(L, -2, "kirim");
        lua_pushcfunction(L, luai_http_post);
        lua_setfield(L, -2, "post");

        // PUT
        lua_pushcfunction(L, luai_http_put);
        lua_setfield(L, -2, "taruh");
        lua_pushcfunction(L, luai_http_put);
        lua_setfield(L, -2, "put");

        // DELETE
        lua_pushcfunction(L, luai_http_delete);
        lua_setfield(L, -2, "hapus");
        lua_pushcfunction(L, luai_http_delete);
        lua_setfield(L, -2, "delete");

        // General REQUEST
        lua_pushcfunction(L, luai_http_request);
        lua_setfield(L, -2, "permintaan");
        lua_pushcfunction(L, luai_http_request);
        lua_setfield(L, -2, "request");

        // Set global
        lua_pushvalue(L, -1);
        lua_setglobal(L, modName);

        // Set package.loaded[modName]
        lua_getglobal(L, "package");
        if (lua_istable(L, -1)) {
            lua_getfield(L, -1, "loaded");
            if (lua_istable(L, -1)) {
                lua_pushvalue(L, -3);
                lua_setfield(L, -2, modName);
            }
            lua_pop(L, 1);
        }
        lua_pop(L, 2); // pop package dan table modul
    };

    setupHttpTable("https");
    setupHttpTable("http");
}
