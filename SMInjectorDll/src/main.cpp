#define _CRT_SECURE_NO_WARNINGS

#include <process.h>
#include <windows.h>
#include <stdio.h>

#include "hook.h"
#include "lua.h"

BOOL InjectLua();

FILE* console_handle;

BOOL Startup(HMODULE hModule) {
    freopen_s(&console_handle, "CONOUT$", "w", stdout);
    
    InjectLua();
    
    MessageBox(0, L"From DLL\n", L"Pausing exit", MB_ICONINFORMATION);
    if(true) return true;
    if(console_handle) {
        fclose(console_handle);
    }

    FreeLibraryAndExitThread(hModule, 0);
    return true;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {
    switch(fdwReason)  { 
        case DLL_PROCESS_ATTACH:
            MessageBox(0, L"From DLL\n", L"Process Attach", MB_ICONINFORMATION);
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Startup, hModule, NULL, NULL);
            break;

        case DLL_PROCESS_DETACH:
            MessageBox(0, L"[DLL_PROCESS_DETACH]\n", L"Pausing exit", MB_ICONINFORMATION);
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}

// LUAL_REGISTER
typedef void (*pluaL_register)(lua_State*, const char*, const luaL_Reg*);
Hook hck_luaL_register;

void hook_luaL_register(lua_State *L, const char *libname, const luaL_Reg *l) {
	printf("hook_luaL_register: libname=[%s]\n", libname);
	
	return ((pluaL_register)hck_luaL_register.Gate())(L, libname, l);
}

// LUAL_LOADSTRING
typedef int (*pluaL_loadstring)(lua_State*, const char*);
Hook hck_luaL_loadstring;

int hook_luaL_loadstring(lua_State *L, const char *s) {
	printf("hook_luaL_loadstring: s=[%s]\n", s);
	
	return ((pluaL_loadstring)hck_luaL_loadstring.Gate())(L, s);
}

// LUA_NEWSTATE
typedef lua_State *(*plua_newstate)(lua_Alloc, void*);
Hook hck_lua_newstate;

lua_State *hook_lua_newstate(lua_Alloc f, void* ud) {
	printf("hck_lua_newstate: ud=[%p]\n", ud);
	
	return ((plua_newstate)hck_lua_newstate.Gate())(f, ud);
}

// LUAL_LOADBUFFER
typedef int (*pluaL_loadbuffer)(lua_State*, const char*, size_t, const char*);
Hook hck_luaL_loadbuffer;

int hook_luaL_loadbuffer(lua_State *L, const char *buff, size_t sz, const char *name) {
	printf("hck_luaL_loadbuffer: buff=[%s], sz=[%zu], name=[%s]\n", buff, sz, name);
	
	return ((pluaL_loadbuffer)hck_luaL_loadbuffer.Gate())(L, buff, sz, name);
}


BOOL InjectLua() {
    printf("Trying to inject the detour\n");

    if(!hck_luaL_register.InjectFromName("lua51.dll", "luaL_register", &hook_luaL_register, 15)) {
        printf("Failed to inject 'luaL_register'\n");
        return false;
    }

    if(!hck_luaL_loadstring.InjectFromName("lua51.dll", "luaL_loadstring", &hook_luaL_loadstring, 16)) {
        printf("Failed to inject 'luaL_loadstring'\n");
        return false;
    }

    if(!hck_lua_newstate.InjectFromName("lua51.dll", "lua_newstate", &hook_lua_newstate, -14, 6)) {
        printf("Failed to inject 'lua_newstate'\n");
        return false;
    }

    if(!hck_luaL_loadbuffer.InjectFromName("lua51.dll", "luaL_loadbuffer", &hook_luaL_loadbuffer, 17, 13)) {
        printf("Failed to inject 'luaL_loadbuffer'\n");
        return false;
    }

    return true;
}