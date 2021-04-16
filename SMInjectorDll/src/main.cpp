#define _CRT_SECURE_NO_WARNINGS

#include <process.h>
#include <windows.h>
#include <stdio.h>

#include "hook.h"
#include "fmod.h"
#include "lua.h"

BOOL PostConsoleInjections();
BOOL InjectLua();
BOOL InjectFMOD();

FILE* console_handle;
HookUtility* util;

#include "hooks.h"

BOOL Startup(HMODULE hModule) {
    hck_init_console = new Hook();
    hck_init_console->Inject((void*)((longlong)GetModuleHandleA("ScrapMechanic.exe") + 0x1b5090), &hooks::hook_init_console, 15);

    MessageBox(0, L"[Press OK to uninject]\n", L"[Pausing]", MB_ICONINFORMATION);

    util->Unload();
    delete util;

    if(console_handle) {
        fclose(console_handle);
    }

    FreeLibraryAndExitThread(hModule, 0);
    return true;
}

BOOL PostConsoleInjections() {
    freopen_s(&console_handle, "CONOUT$", "w", stdout);
    printf("[INJECTING THE HANDLER]\n");
    util = new HookUtility();

    InjectLua();
    /*
    if(!InjectFMOD()) {
        printf("Failed to execute [InjectFMOD]\n");
    }
    */

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

BOOL InjectLua() {
    printf("Trying to inject the detour\n");

    hck_luaL_register = util->InjectFromName("lua51.dll", "luaL_register", &hooks::hook_luaL_register, 15);
    hck_luaL_loadstring = util->InjectFromName("lua51.dll", "luaL_loadstring", &hooks::hook_luaL_loadstring, 16);
    hck_lua_newstate = util->InjectFromName("lua51.dll", "lua_newstate", &hooks::hook_lua_newstate, -14, 6);
    hck_luaL_loadbuffer = util->InjectFromName("lua51.dll", "luaL_loadbuffer", &hooks::hook_luaL_loadbuffer, 17, 13);
    
    if(!hck_luaL_register) {
        printf("Failed to inject 'luaL_register'\n");
        return false;
    }

    if(!hck_luaL_loadstring) {
        printf("Failed to inject 'luaL_loadstring'\n");
        return false;
    }

    if(!hck_lua_newstate) {
        printf("Failed to inject 'lua_newstate'\n");
        return false;
    }

    if(!hck_luaL_loadbuffer) {
        printf("Failed to inject 'luaL_loadbuffer'\n");
        return false;
    }

    return true;
}

typedef FMOD_RESULT (*pFMOD_System_CreateSound)(void *_this, const char *name_or_data, int mode, void *exinfo, void **sound);
pFMOD_System_CreateSound FMOD_System_CreateSound;

typedef FMOD_RESULT (*pFMOD_System_PlaySound)(void *_this, void *sound, void *channelgroup, bool paused, void **channel);
pFMOD_System_PlaySound FMOD_System_PlaySound;

BOOL InjectFMOD() {
    printf("Trying to access FMOD functions\n");

    void* system_0 = NULL;
    void* system_1 = NULL;

    try {
        HMODULE hModule = GetModuleHandleA("ScrapMechanic.exe");
        if(!hModule) return false;
        
        longlong p0 = *(longlong*)((longlong)hModule + 0xE55C40);
        longlong p1 = *(longlong*)(p0 + 0x2C8);
        longlong p2 = *(longlong*)(p1 + 0x70);
        system_1 = (void*)p2;
        
        printf("SM: p1 -> [%p]\n", (void*)p1);
    } catch(...) {
        printf("Failed to execute pointer code\n");
    }

    printf("Read FMOD system pointers: [%p] [%p]\n", system_0, system_1);

    try {
        HMODULE hModule = GetModuleHandleA("fmod.dll");
        if(!hModule) return false;

        const char *path = "C:\\Users\\Admin\\source\\repos\\SMInjectorProject\\SMInjector\\x64\\Release\\spooky.mp3";
        printf("FMOD: path='%s'\n", path);

        FMOD_System_CreateSound = (pFMOD_System_CreateSound)GetProcAddress(hModule, "FMOD_System_CreateSound");
        printf("FMOD: FMOD_System_CreateSound -> [%p]\n", FMOD_System_CreateSound);
        FMOD_System_PlaySound = (pFMOD_System_PlaySound)GetProcAddress(hModule, "FMOD_System_PlaySound");
        printf("FMOD: FMOD_System_PlaySound -> [%p]\n", FMOD_System_PlaySound);

        void *sound = NULL;
        FMOD_RESULT result = FMOD_System_CreateSound(system_1, path, FMOD_DEFAULT, NULL, &sound);
        printf("FMOD: sound=[%p], result=[%d]\n", sound, result);

        if(result == 0) { // FMOD_OK
            void *channel = NULL;
            FMOD_RESULT result_2 = FMOD_System_PlaySound(system_1, sound, NULL, false, &channel);
            printf("FMOD: channel=[%p], result_2=[%d]\n", channel, result_2);

        }
    } catch(...) {
        printf("Failed to get proc\n");
    }

    return true;
}