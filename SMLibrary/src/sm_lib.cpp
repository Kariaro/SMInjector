#include <process.h>
#include <windows.h>
#include <stdio.h>
#include <vector>

#define _SM_PLUGIN_NAME SMLibrary

#include "../include/sm_lib.h"
#include "../include/hook.h"
#include "../include/fmod.h"
#include "../include/lua.h"

#include "../include/console.h"
using Console::Color;

struct LIB_PLUGIN {
	const char *plugin_name = NULL;
	HMODULE hModule = NULL;
	LIB_CALLBACK load = NULL;
	LIB_CALLBACK unload = NULL;
};

std::vector<LIB_PLUGIN> plugins;
_LIB_EXPORT void InjectPlugin(void* hModule, const char *plugin_name, LIB_CALLBACK load, LIB_CALLBACK unload) {
	LIB_PLUGIN plugin = { plugin_name, (HMODULE)hModule, load, unload };
	plugins.push_back(plugin);
}

BOOL PostConsoleInjections();
BOOL InjectLua();
BOOL InjectPlayground();
BOOL InjectFMOD();

HookUtility *util;

#include "hooks.h"

BOOL Startup(HMODULE hModule) {
	// This hooks is not going to be unloaded

	HMODULE sm_handle = GetModuleHandleA("ScrapMechanic.exe");
	if(!sm_handle) return false;
	
	hck_init_console = new Hook();
	hck_init_console->Inject((void*)((longlong)sm_handle + 0x1b5090), &Hooks::hook_init_console, 15);

	// MessageBox(0, L"[Press OK to uninject Injected The Terrain TileReader]\n", L"[Pausing]", MB_ICONINFORMATION);
	// util->Unload();
	//delete util;
	//Console::log_close();
	//FreeLibraryAndExitThread(hModule, 0);
	return true;
}

BOOL PostConsoleInjections() {
	if(true) return;

	Console::log_open();
	Console::log(Color::Aqua, "[SMLibrary]: Installing the library functions");
	util = new HookUtility();
	InjectLua();
	InjectPlayground();

	const size_t plugins_size = plugins.size();
	Console::log(Color::Aqua, "[SMLibrary]: Found '%d' plugin(s)", plugins_size);
	Console::log(Color::Aqua, "[SMLibrary]:");
	
	for(int i = 0; i < plugins_size; i++) {
		LIB_PLUGIN plugin = plugins[i];
		Console::log(Color::LightYellow, "[SMLibrary]: Initializing plugin '%s'", plugin.plugin_name);

		// Init the plugin
		plugin.load();
	}

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
	Console::log(Color::Aqua, "[SMLibrary]: Installing lua hooks");

	hck_luaL_register = util->InjectFromName("lua51.dll", "luaL_register", &Hooks::hook_luaL_register, 15);
	hck_luaL_loadstring = util->InjectFromName("lua51.dll", "luaL_loadstring", &Hooks::hook_luaL_loadstring, 16);
	hck_lua_newstate = util->InjectFromName("lua51.dll", "lua_newstate", &Hooks::hook_lua_newstate, -14, 6);
	hck_luaL_loadbuffer = util->InjectFromName("lua51.dll", "luaL_loadbuffer", &Hooks::hook_luaL_loadbuffer, 17, 13);
	
	if(!hck_luaL_register) {
		Console::log(Color::Red, "[SMLibrary]: Failed to inject 'luaL_register'");
		return false;
	}

	if(!hck_luaL_loadstring) {
		Console::log(Color::Red, "[SMLibrary]: Failed to inject 'luaL_loadstring'");
		return false;
	}

	if(!hck_lua_newstate) {
		Console::log(Color::Red, "[SMLibrary]: Failed to inject 'lua_newstate'");
		return false;
	}

	if(!hck_luaL_loadbuffer) {
		Console::log(Color::Red, "[SMLibrary]: Failed to inject 'luaL_loadbuffer'");
		return false;
	}

	return true;
}

BOOL InjectPlayground() {
	// Testing

	return true;
}

typedef FMOD_RESULT (*pFMOD_System_CreateSound)(void *_this, const char *name_or_data, int mode, void *exinfo, void **sound);
pFMOD_System_CreateSound FMOD_System_CreateSound;

typedef FMOD_RESULT (*pFMOD_System_PlaySound)(void *_this, void *sound, void *channelgroup, bool paused, void **channel);
pFMOD_System_PlaySound FMOD_System_PlaySound;

BOOL InjectFMOD() {
	Console::log(Color::White, "[SMLibrary]: Trying to access FMOD functions");

	void* system_1 = NULL;

	try {
		HMODULE hModule = GetModuleHandleA("ScrapMechanic.exe");
		if(!hModule) return false;
		
		longlong p0 = *(longlong*)((longlong)hModule + 0xE55C40);
		longlong p1 = *(longlong*)(p0 + 0x2C8);
		longlong p2 = *(longlong*)(p1 + 0x70);
		system_1 = (void*)p2;
		
		Console::log(Color::White, "[SMLibrary]: SM: p1 -> [%p]", (void*)p1);
	} catch(...) {
		Console::log(Color::Red, "[SMLibrary]: Failed to execute pointer code");
	}

	Console::log(Color::White, "[SMLibrary]: Read FMOD system pointers: [%p]", system_1);

	try {
		HMODULE hModule = GetModuleHandleA("fmod.dll");
		if(!hModule) return false;

		const char *path = "C:\\Users\\Admin\\source\\repos\\SMInjectorProject\\SMInjector\\x64\\Release\\spooky.mp3";
		Console::log(Color::White, "[SMLibrary]: FMOD: path='%s'", path);

		FMOD_System_CreateSound = (pFMOD_System_CreateSound)GetProcAddress(hModule, "FMOD_System_CreateSound");
		Console::log(Color::White, "[SMLibrary]: FMOD: FMOD_System_CreateSound -> [%p]", FMOD_System_CreateSound);
		FMOD_System_PlaySound = (pFMOD_System_PlaySound)GetProcAddress(hModule, "FMOD_System_PlaySound");
		Console::log(Color::White, "[SMLibrary]: FMOD: FMOD_System_PlaySound -> [%p]", FMOD_System_PlaySound);

		void *sound = NULL;
		FMOD_RESULT result = FMOD_System_CreateSound(system_1, path, FMOD_DEFAULT, NULL, &sound);
		Console::log(Color::White, "[SMLibrary]: FMOD: sound=[%p], result=[%d]", sound, result);

		if(result == 0) { // FMOD_OK
			void *channel = NULL;
			FMOD_RESULT result_2 = FMOD_System_PlaySound(system_1, sound, NULL, false, &channel);
			Console::log(Color::White, "[SMLibrary]: FMOD: channel=[%p], result_2=[%d]", channel, result_2);

		}
	} catch(...) {
		Console::log(Color::Red, "[SMLibrary]: Failed to get proc");
	}

	return true;
}
