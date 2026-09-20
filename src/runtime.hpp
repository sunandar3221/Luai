#ifndef LUAI_RUNTIME_HPP
#define LUAI_RUNTIME_HPP

#include <string>
#include <vector>

struct lua_State;

// Informasi engine yang benar-benar dipakai (LuaJIT atau Lua 5.4).
// Dipakai oleh main.cpp & repl.cpp supaya teks versi tidak menyesatkan.
const char* luaiEngineName();
const char* luaiEngineTagline();
const char* luaiBuildPlatform();
bool luaiJitEnabled(lua_State* L);

class LuaiRuntime {
public:
    LuaiRuntime();
    ~LuaiRuntime();

    bool init();
    bool executeString(const std::string& code, const std::string& chunkName = "luai");
    bool executeFile(const std::string& filepath, const std::vector<std::string>& args = {});
    bool evaluateExpression(const std::string& expr, std::string& output);

    lua_State* getState() const;
    std::string getLastError() const;
    static std::string formatError(const std::string& rawError);

private:
    lua_State* L;
    std::string lastError;

    void registerIndonesianBindings();
    void registerModuleAliases();
    void registerSearcher();
    static int luaiSearcher(lua_State* L);
};

#endif
