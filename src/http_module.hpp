#ifndef LUAI_HTTP_MODULE_HPP
#define LUAI_HTTP_MODULE_HPP

struct lua_State;

// Mendaftarkan modul 'https' dan alias 'http' ke dalam runtime Luai
void luai_register_http_module(lua_State* L);

#endif // LUAI_HTTP_MODULE_HPP
