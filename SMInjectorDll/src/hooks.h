
#include <stdio.h>
#include "hook.h"
#include "fmod.h"
#include "lua.h"

// LUAL_REGISTER
typedef void (*pluaL_register)(lua_State*, const char*, const luaL_Reg*);
Hook *hck_luaL_register;

// LUAL_LOADSTRING
typedef int (*pluaL_loadstring)(lua_State*, const char*);
Hook *hck_luaL_loadstring;

// LUA_NEWSTATE
typedef lua_State *(*plua_newstate)(lua_Alloc, void*);
Hook *hck_lua_newstate;

// LUAL_LOADBUFFER
typedef int (*pluaL_loadbuffer)(lua_State*, const char*, size_t, const char*);
Hook *hck_luaL_loadbuffer;


// INIT_CONSOLE
typedef void (*pinit_console)(void*, void*);
Hook *hck_init_console;

// =============

namespace hooks {
	void hook_luaL_register(lua_State *L, const char *libname, const luaL_Reg *l) {
		printf("hook_luaL_register: libname=[%s]\n", libname);
		return ((pluaL_register)hck_luaL_register->Gate())(L, libname, l);
	}

	int hook_luaL_loadstring(lua_State *L, const char *s) {
		printf("hook_luaL_loadstring: s=[%s]\n", s);
		return ((pluaL_loadstring)hck_luaL_loadstring->Gate())(L, s);
	}

	lua_State *hook_lua_newstate(lua_Alloc f, void* ud) {
		printf("hck_lua_newstate: ud=[%p]\n", ud);
		return ((plua_newstate)hck_lua_newstate->Gate())(f, ud);
	}

	int hook_luaL_loadbuffer(lua_State *L, const char *buff, size_t sz, const char *name) {
		printf("hck_luaL_loadbuffer: buff=[ ... ], sz=[%zu], name=[%s]\n", sz, name);
		return ((pluaL_loadbuffer)hck_luaL_loadbuffer->Gate())(L, buff, sz, name);
	}

	void hook_init_console(void* a, void* b) {
		((pinit_console)hck_init_console->Gate())(a, b);
		PostConsoleInjections();
		return;
	}
}