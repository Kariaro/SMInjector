#include <stdio.h>
#include "../include/hook.h"
#include "../include/fmod.h"
#include "../include/lua.h"

#include "../include/console.h"
using Console::Color;

typedef unsigned long long ulonglong;


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



// MYGUI_GET_PARENT
//typedef int (*pMyGUI_Widget_getParent)(void*, void*);
//Hook *hck_MyGUI_Widget_getParent;

typedef void(*pToggleTileEditorWindow)(void*, char);
Hook *hck_ToggleTileEditorWindow;

typedef void(*pToggleTileEditorSetWindow)(void*, char);
Hook *hck_ToggleTileEditorSetWindow;

// =============

namespace Hooks {
	/*
	void* get_global_tile_editor() {
		HMODULE sm_handle = GetModuleHandleA("ScrapMechanic.exe");
		if(!sm_handle) return nullptr;

		//0xe6e788;
		return (void*)*(longlong*)((longlong)sm_handle + 0xe6e788);
	}

	void AddEntityToMemory(void* a, _vector_ptr vec) {
		typedef void(*add_entity)(void*, void*, int);

		HMODULE sm_handle = GetModuleHandleA("ScrapMechanic.exe");
		if(sm_handle) {
			// AddEntityToMemory_???_1408fb2c0(paVar1,local_420.begin,(int)((longlong)((longlong)local_420.index - (longlong)local_420.begin) >> 5) * -0x33333333);

			int value = ((int)(vec[1] - vec[0]) >> 5) * -0x33333333;
			((add_entity)((longlong)sm_handle + 0x8fb2c0))(a, (void*)vec[0], value);
		}
	}

	void hook_ReadNode(void* a, _vector_ptr vec, void* b, void* c, void* d, int e) {
		Console::log(Color::Aqua, "hook_ReadNode: a=[%p], vec=[%p], b=[%p], c=[%p], d=[%p], e=[%d]", a, vec, b, c, d, e);

		((pReadNode)hck_ReadNode->Gate())(a, vec, b, c, d, e);

		void* GLOBAL_TILE_EDITOR = get_global_tile_editor();
		Console::log(Color::Aqua, "hook_ReadNode: GLOBAL_TILE_EDITOR=[%p]", GLOBAL_TILE_EDITOR);

		if(GLOBAL_TILE_EDITOR) {
			void* field_astruct_entities_0xf8 = *(void**)((longlong)GLOBAL_TILE_EDITOR + 0xf8);
			AddEntityToMemory(field_astruct_entities_0xf8, vec);
		}

		// Vec contains a vector that should be handed over to the Entity manager
		return;
	}

	void hook_ReadBlueprintList(int a, _vector_ptr vec, void* b, void* c, void* d, int e, void* f, int g) {
		Console::log(Color::Aqua, "hook_ReadBlueprintList: a=[%d], vec=[%p], b=[%p], c=[%p], d=[%p], e=[%d], f=[%p], g=[%d]", a, vec, b, c, d, e, f, g);

		((pReadBlueprintList)hck_ReadBlueprintList->Gate())(a, vec, b, c, d, e, f, g);

		void* GLOBAL_TILE_EDITOR = get_global_tile_editor();
		Console::log(Color::Aqua, "hook_ReadBlueprintList: GLOBAL_TILE_EDITOR=[%p]", GLOBAL_TILE_EDITOR);

		if(GLOBAL_TILE_EDITOR) {
			void* field_astruct_entities_0xf8 = *(void**)((longlong)GLOBAL_TILE_EDITOR + 0xf8);
			AddEntityToMemory(field_astruct_entities_0xf8, vec);
		}

		return;
	}
	*/

	
	void hook_luaL_register(lua_State *L, const char *libname, const luaL_Reg *l) {
		Console::log(Color::Aqua, "hook_luaL_register: libname=[%s]", libname);
		return ((pluaL_register)hck_luaL_register->Gate())(L, libname, l);
	}

	int hook_luaL_loadstring(lua_State *L, const char *s) {
		Console::log(Color::Aqua, "hook_luaL_loadstring: s=[ ... ]");
		return ((pluaL_loadstring)hck_luaL_loadstring->Gate())(L, s);
	}

	lua_State *hook_lua_newstate(lua_Alloc f, void* ud) {
		Console::log(Color::Aqua, "hck_lua_newstate: ud=[%p]", ud);
		return ((plua_newstate)hck_lua_newstate->Gate())(f, ud);
	}

	int hook_luaL_loadbuffer(lua_State *L, const char *buff, size_t sz, const char *name) {
		Console::log(Color::Aqua, "hck_luaL_loadbuffer: buff=[ ... ], sz=[%zu], name=[%s]", sz, name);
		return ((pluaL_loadbuffer)hck_luaL_loadbuffer->Gate())(L, buff, sz, name);
	}

	void hook_init_console(void* a, void* b) {
		((pinit_console)hck_init_console->Gate())(a, b);
		PostConsoleInjections();
		return;
	}

	/*
	longlong hook_MyGUI_Widget_getParent(void* widget, void* param_1) {
		Console::log(Color::Aqua, "hook_MyGUI_Widget_getParent: this=[%p], param_1=[%p]", widget, param_1);
		
		return ((pMyGUI_Widget_getParent)hck_MyGUI_Widget_getParent->Gate())(widget, param_1);
	}
	*/

	#define GET_FIELD(ptr, index) (void*)(*(longlong*)((longlong)ptr + index))
	#define GET_WIDGET(ptr) (void*)(*(longlong*)((longlong)ptr + 8))
	
	typedef void (*pWidgetShow)(void*, longlong);
	#define GET_WIDGET_SHOW(ptr) ((pWidgetShow)GET_FIELD(*(longlong*)ptr, 0x18))

	
	#define SHOW_WIDGET(ptr, index, name) \
		{ \
			void* _tmp = GET_FIELD(ptr, index); \
			void* _wid = GET_WIDGET(_tmp); \
			pWidgetShow _func = GET_WIDGET_SHOW(_wid); \
			Console::log(Color::Aqua, "    : %-26s=[%p]", name, (void*)_func);\
			_func(_wid, 1); \
		}

	#define SHOW_WIDGET_SEMI(ptr, index, name) \
		{ \
			void* _tmp = GET_FIELD(ptr, index); \
			void* _wid = GET_FIELD(_tmp, 0); \
			pWidgetShow _func = GET_WIDGET_SHOW(_wid); \
			Console::log(Color::Aqua, "    : %-26s=[%p]", name, (void*)_func);\
			_func(_wid, 1); \
		}

	#define SHOW_WIDGET_DIRECT(ptr, index, name) \
		{ \
			void* _tmp = GET_FIELD(ptr, index); \
			pWidgetShow _func = GET_WIDGET_SHOW(_tmp); \
			Console::log(Color::Aqua, "    : %-26s=[%p]", name, (void*)_func);\
			_func(_tmp, 1); \
		}

	static int testing = 0;
	void hook_ToggleTileEditorWindow(void* tile_editor, char index) {
		Console::log(Color::Aqua, "hook_ToggleTileEditorWindow: tile_editor=[%p], index=[%d]", tile_editor, index);
		
		if(index != 15) {
			void* tile_editor_window = GET_FIELD(tile_editor, 208);
			Console::log(Color::Aqua, "    : tile_editor_window=[%p]", tile_editor_window);
			
			/*
			void* field_NodeLibraryWindow = GET_FIELD(tile_editor_window, 96);
			void* field_ShapeLibraryWindow = GET_FIELD(tile_editor_window, 112);
			void* field_ImportCreationWindow = GET_FIELD(tile_editor_window, 128);

			void* node_widget = GET_WIDGET(field_NodeLibraryWindow);
			void* shape_widget = GET_WIDGET(field_ShapeLibraryWindow);
			void* creation_widget = GET_WIDGET(field_ImportCreationWindow);
			*/

			SHOW_WIDGET(tile_editor_window, 0x58, "AssetLibraryWindow");
			SHOW_WIDGET(tile_editor_window, 0x60, "NodeLibraryWindow");
			SHOW_WIDGET(tile_editor_window, 0x68, "HarvestableLibraryWindow");
			SHOW_WIDGET(tile_editor_window, 0x70, "ShapeLibraryWindow");
			SHOW_WIDGET(tile_editor_window, 0x78, "GroundMaterialWindow");
			SHOW_WIDGET(tile_editor_window, 0x80, "ImportCreationWindow");
			

			SHOW_WIDGET_SEMI(tile_editor_window, 0x88, "PropertiesWindow");
			SHOW_WIDGET_SEMI(tile_editor_window, 0x98, "BrushWindow");
			SHOW_WIDGET_SEMI(tile_editor_window, 0xa8, "ColorEditor");

			SHOW_WIDGET_DIRECT(tile_editor_window, 0xd8, "TerrainBrushWindow");
			SHOW_WIDGET_DIRECT(tile_editor_window, 0xe8, "ClutterLibraryWindow");

			index = testing++;
			if(testing >= 15) {
				testing = 0;
			}
			Console::log(Color::Aqua, "    : Testing=%d", testing);
		}
		
		return ((pToggleTileEditorWindow)hck_ToggleTileEditorWindow->Gate())(tile_editor, index);
	}

	void hook_ToggleTileEditorSetWindow(void* tile_editor, char index) {
		Console::log(Color::LightAqua, "hook_ToggleTileEditorSetWindow: tile_editor=[%p], index=[%d]", tile_editor, index);
		/*
		if(index != 15) {
			void* tile_editor_window = GET_FIELD(tile_editor, 208);
			Console::log(Color::LightAqua, "    : tile_editor_window=[%p]", tile_editor_window);
			
			void* field_NodeLibraryWindow = GET_FIELD(tile_editor_window, 96);
			void* field_ShapeLibraryWindow = GET_FIELD(tile_editor_window, 112);
			void* field_ImportCreationWindow = GET_FIELD(tile_editor_window, 128);

			void* node_widget = GET_WIDGET(field_NodeLibraryWindow);
			void* shape_widget = GET_WIDGET(field_ShapeLibraryWindow);
			void* creation_widget = GET_WIDGET(field_ImportCreationWindow);
			
			SHOW_WIDGET(tile_editor_window, 0x58, "AssetLibraryWindow");
			SHOW_WIDGET(tile_editor_window, 0x60, "NodeLibraryWindow");
			SHOW_WIDGET(tile_editor_window, 0x68, "HarvestableLibraryWindow");
			SHOW_WIDGET(tile_editor_window, 0x70, "ShapeLibraryWindow");
			SHOW_WIDGET(tile_editor_window, 0x78, "GroundMaterialWindow");
			SHOW_WIDGET(tile_editor_window, 0x80, "ImportCreationWindow");
			

			SHOW_WIDGET_SEMI(tile_editor_window, 0x88, "PropertiesWindow");
			SHOW_WIDGET_SEMI(tile_editor_window, 0x98, "BrushWindow");
			SHOW_WIDGET_SEMI(tile_editor_window, 0xa8, "ColorEditor");

			SHOW_WIDGET_DIRECT(tile_editor_window, 0xd8, "TerrainBrushWindow");
			SHOW_WIDGET_DIRECT(tile_editor_window, 0xe8, "ClutterLibraryWindow");

			index = testing++;
			if(testing >= 15) {
				testing = 0;
			}
			Console::log(Color::LightAqua, "    : Testing=%d", testing);
		}
		*/
		return ((pToggleTileEditorSetWindow)hck_ToggleTileEditorSetWindow->Gate())(tile_editor, index);
	}
}
