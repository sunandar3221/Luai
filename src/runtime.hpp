#ifndef LUAI_RUNTIME_HPP
#define LUAI_RUNTIME_HPP

#include <string>
#include <vector>

struct lua_State;

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

private:
    lua_State* L;
    std::string lastError;

    void registerIndonesianBindings();
    void registerModuleAliases();
    void registerSearcher();
    static int luaiSearcher(lua_State* L);
};

#endif
