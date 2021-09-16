#define _SM_LIBRARY_BUILD_PLUGIN
#define _SM_PLUGIN_NAME SMLuaHook

#include <sm_lib.h>
#include <console.h>
#include <sigscan.h>
#include <plugin_config.h>

using Console::Color;

#include "hooks.h"
bool InjectLua();

LIB_RESULT PluginLoad() {
	Console::log(Color::Aqua, "Starting plugin ...");

	if (!InjectLua()) {
		Console::log(Color::Red, "Failed to inject lua functions");
		return PLUGIN_ERROR;
	}

	return PLUGIN_SUCCESSFULL;
}

const char* defaultConfig = R"(// Configuration file for hooking the Lua C API
{
	"hooks": {
		"luaL_loadstring": [
			{
				"selector": {
					"operator": "contains",
					"value": "unsafe_env"
				},
				"file": {
					"option": "replace_content",
					"name": "$PLUGIN_CONFIG/override/unsafe_env.lua"
				}
			}
		],
		"luaL_loadbuffer": [
			{
				"selector": {
				    "field": "name",
				    "operator": "equals",
				    "value": "Survival/Scripts/game/SurvivalGame.lua"
				},
				"file": {
					"option": "replace_content",
					"name": "$PLUGIN_CONFIG/override/files/SurvivalGame.lua"
				}
			}
		]
	}
}
)";

bool InjectLua() {
	Console::log(Color::Aqua, "Installing lua hooks");

	hck_luaL_register = GameHooks::InjectFromName("lua51.dll", "luaL_register", &Hooks::hook_luaL_register, 15);
	hck_luaL_loadstring = GameHooks::InjectFromName("lua51.dll", "luaL_loadstring", &Hooks::hook_luaL_loadstring, 16);
	hck_lua_newstate = GameHooks::InjectFromName("lua51.dll", "lua_newstate", &Hooks::hook_lua_newstate, 6);
	hck_luaL_loadbuffer = GameHooks::InjectFromName("lua51.dll", "luaL_loadbuffer", &Hooks::hook_luaL_loadbuffer, 13);

	if (!hck_luaL_register) {
		Console::log(Color::Red, "Failed to inject 'luaL_register'");
		return false;
	}

	if (!hck_luaL_loadstring) {
		Console::log(Color::Red, "Failed to inject 'luaL_loadstring'");
		return false;
	}

	if (!hck_lua_newstate) {
		Console::log(Color::Red, "Failed to inject 'lua_newstate'");
		return false;
	}

	if (!hck_luaL_loadbuffer) {
		Console::log(Color::Red, "Failed to inject 'luaL_loadbuffer'");
		return false;
	}

	PluginConfig config(_LIB_PLUGIN_NAME_STR, "lua_hooks.json");
	config.setDefaultContent(defaultConfig);
	config.createIfNotExists();
	config.load();

	return true;
}

LIB_RESULT PluginUnload() {
	Console::log(Color::Aqua, "Unloading this plugin!");
	//util->Unload();
	return PLUGIN_SUCCESSFULL;
}
