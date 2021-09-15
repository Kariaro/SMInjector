#include <process.h>
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

#define _SM_PLUGIN_NAME SMLibrary
#define _SM_OUTPUT_LOGS

#include "../include/sm_lib.h"
#include "../include/hook.h"
#include "../include/plugin_config.h"

#include "../include/console.h"
using Console::Color;

constexpr longlong offset_InitConsole = 0x1b5410;
//0x1b5090;

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
HookUtility *util;

#include "hooks.h"

fs::path smlibrarydllPath;

BOOL Startup(HMODULE hModule) {
	HMODULE sm_handle = GetModuleHandleA("ScrapMechanic.exe");
	if(!sm_handle) return false;

	TCHAR dllPath[MAX_PATH] = { 0 };
	DWORD length = GetModuleFileName(hModule, dllPath, _countof(dllPath));
	smlibrarydllPath = fs::path(dllPath);

	hck_init_console = new Hook();
	hck_init_console->Inject((void*)((longlong)sm_handle + offset_InitConsole), &Hooks::hook_init_console, 0, 15);
	return true;
}

BOOL PostConsoleInjections() {
	Console::log_open();

	PluginConfig::setConfigDirectory(smlibrarydllPath.parent_path() / "config");

	Console::log(Color::Aqua, "Installing the library functions");
	util = new HookUtility();

	const size_t plugins_size = plugins.size();
	Console::log(Color::Aqua, "Found '%d' plugin(s)", plugins_size);
	Console::log(Color::Aqua, "");
	
	for(int i = 0; i < plugins_size; i++) {
		LIB_PLUGIN plugin = plugins[i];
		Console::log(Color::LightYellow, "Initializing plugin '%s'", plugin.plugin_name);

		// Init the plugin
		plugin.load();
	}

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
