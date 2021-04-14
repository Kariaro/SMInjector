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

    if(console_handle) {
        fclose(console_handle);
    }

    if(true) return true;
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
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}

typedef void (*pluaL_register)(lua_State*, const char*, const luaL_Reg*);
Hook hck_luaL_register;

void hook_luaL_register(lua_State *L, const char *libname, const luaL_Reg *l) {
	printf("hook_luaL_register :O libname=%s\n", libname);
	
	return ((pluaL_register)hck_luaL_register.Gate())(L, libname, l);
}

BOOL InjectLua() {
    printf("Trying to inject the detour\n");

    if(!hck_luaL_register.Inject("lua51.dll", "luaL_register", &hook_luaL_register, 15)) {
        printf("Failed to inject 'luaL_register'\n");
        return false;
    }

    return true;
}