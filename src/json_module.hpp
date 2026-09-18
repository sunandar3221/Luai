#ifndef LUAI_JSON_MODULE_HPP
#define LUAI_JSON_MODULE_HPP

struct lua_State;

// Mendaftarkan modul 'json' ke dalam runtime Luai
void luai_register_json_module(lua_State* L);

#endif // LUAI_JSON_MODULE_HPP
