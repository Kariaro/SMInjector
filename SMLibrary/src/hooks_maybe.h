
#include <stdio.h>
#include "hook.h"
#include "fmod.h"
#include "lua.h"

#ifndef _hook_function
#define _hook_function(name, type, ...) \
	typedef type (*name)(__VA_ARGS__);\
	Hook hck_##name;\
	type hook_##name(__VA_ARGS__)
#endif

// LUAL_REGISTER
_hook_function(luaL_register, void, lua_State *L, const char *libname, const luaL_Reg *l) {
	printf("hook_luaL_register: libname=[%s]\n", libname);
	
	return ((luaL_register)hck_luaL_register.Gate())(L, libname, l);
}

// LUAL_LOADSTRING
_hook_function(luaL_loadstring, int, lua_State *L, const char *s) {
	printf("hook_luaL_loadstring: s=[%s]\n", s);
	
	return ((luaL_loadstring)hck_luaL_loadstring.Gate())(L, s);
}

// LUA_NEWSTATE
_hook_function(lua_newstate, lua_State*, lua_Alloc f, void* ud) {
	printf("hck_lua_newstate: ud=[%p]\n", ud);
	
	return ((lua_newstate)hck_lua_newstate.Gate())(f, ud);
}

// LUAL_LOADBUFFER
_hook_function(luaL_loadbuffer, int, lua_State *L, const char *buff, size_t sz, const char *name) {
	printf("hck_luaL_loadbuffer: buff=[%s], sz=[%zu], name=[%s]\n", buff, sz, name);
	
	return ((luaL_loadbuffer)hck_luaL_loadbuffer.Gate())(L, buff, sz, name);
}


// FMOD::Studio::System::getCoreSystem
typedef FMOD_RESULT (*pgetCoreSystem)(void*, void*);
Hook hck_getCoreSystem;

FMOD_RESULT hook_getCoreSystem(void* ptr0, void* ptr1) {
	printf("hck_getCoreSystem: ptr0=[%p], ptr1=[%p]\n", ptr0, ptr1);
	
	return ((pgetCoreSystem)hck_luaL_loadbuffer.Gate())(ptr0, ptr1);
}