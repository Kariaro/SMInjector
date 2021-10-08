#pragma once
#include <stdio.h>
#include <gamehook.h>
#include <path_helper.h>
#include <lua.hpp>

#include <console.h>
using Console::Color;

#include "lua_hook_config.h"


// LUAL_REGISTER
typedef void (*pluaL_register)(lua_State*, const char*, const luaL_Reg*);
GameHook* hck_luaL_register;

// LUAL_LOADSTRING
typedef int (*pluaL_loadstring)(lua_State*, const char*);
GameHook* hck_luaL_loadstring;

// LUA_NEWSTATE
typedef lua_State* (*plua_newstate)(lua_Alloc, void*);
GameHook* hck_lua_newstate;

// LUAL_LOADBUFFER
typedef int (*pluaL_loadbuffer)(lua_State*, const char*, size_t, const char*);
GameHook* hck_luaL_loadbuffer;

// =============

namespace LuaHook::Hooks {
	void hook_luaL_register(lua_State* L, const char* libname, const luaL_Reg* l) {
		Console::log(Color::Aqua, "hook_luaL_register: libname=[%s]", libname);

		const luaL_Reg* ptr = l;

		int i = 0;
		while (ptr->name != NULL) {
			Console::log(Color::Aqua, "hook_luaL_register: luaL_Reg[%d] name=[%s] func=[%p]", i++, ptr->name, (void*)ptr->func);

			ptr++;
		}

		return ((pluaL_register)*hck_luaL_register)(L, libname, l);
	}

	int hook_luaL_loadstring(lua_State* L, const char* s) {
		Console::log(Color::Aqua, "hook_luaL_loadstring: s=[ ... ]");

		std::map<std::string, std::any> fields = {
			{"s", &s}
		};

		for (LuaHook::hookItem& hookItem : LuaHook::HookConfig::getHookItems("luaL_loadstring")) {
			bool selected = true;

			for (LuaHook::selector& selector : hookItem.selector) {
				selected &= (*selector.func)(fields, hookItem, selector);

				Console::log(selected ? Color::LightPurple : Color::Purple, selected ? "selected" : "not selected");

				if (!selected) {
					break;
				}
			}

			if (selected) {
				std::string sStr(s);

				for (LuaHook::executor& executor : hookItem.execute) {
					try {
						if (executor.command == "REPLACE_CONTENT_WITH_FILE") {
							sStr = LuaHook::ExecutorHelper::readFile(PathHelper::resolvePath(executor.j_executor.at("file")));
						}
					}
					catch (std::exception& e) {
						Console::log(Color::LightRed, "Failed executing executor %s: %s", executor.j_executor.dump(4).c_str(), e.what());
					}
				}

				Console::log(Color::Green, "Set contents to:\n%s", sStr.c_str());
				return ((pluaL_loadstring)*hck_luaL_loadstring)(L, sStr.c_str());
			}
		}

		return ((pluaL_loadstring)*hck_luaL_loadstring)(L, s);
	}

	lua_State* hook_lua_newstate(lua_Alloc f, void* ud) {
		Console::log(Color::Aqua, "hck_lua_newstate: ud=[%p]", ud);
		return ((plua_newstate)*hck_lua_newstate)(f, ud);
	}

	int hook_luaL_loadbuffer(lua_State* L, const char* buff, size_t sz, const char* name) {
		Console::log(Color::Aqua, "hck_luaL_loadbuffer: buff=[ ... ], sz=[%zu], name=[%s]", sz, name);

		std::map<std::string, std::any> fields = {
			{"buff", &buff},
			{"name", &name}
		};

		for (LuaHook::hookItem& hookItem : LuaHook::HookConfig::getHookItems("luaL_loadbuffer")) {
			for (LuaHook::selector& selector : hookItem.selector) {
				bool selected = (*selector.func)(fields, hookItem, selector);
				Console::log(selected ? Color::LightPurple : Color::Purple, selected ? "selected" : "not selected");
			}
		}

		return ((pluaL_loadbuffer)*hck_luaL_loadbuffer)(L, buff, sz, name);
	}
}
