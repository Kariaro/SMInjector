#define _SM_LIBRARY_BUILD_PLUGIN
#define _SM_PLUGIN_NAME TileEditorUnlocker

#include <sm_lib.h>
#include <console.h>
using Console::Color;

#include <hook.h>

// debugSize == h.blueprintListCompressedSize

// 0.5.1 Build 660:
//   GLOBAL_TILE_EDITOR: 0xe708f8
//   ADD_ENTITY:         0x8fa950
//   ReadNode:           0x93dfb0
//   ReadBlueprintList:  0x91cb40
constexpr longlong offset_GLOBAL_TILE_EDITOR = 0xe708f8;
constexpr longlong offset_ADD_ENTITY = 0x8fa950;
constexpr longlong offset_ReadNode = 0x93dfb0;
constexpr longlong offset_ReadBlueprintList = 0x91cb40;


namespace Injection {
	typedef longlong* _vector_ptr;

	typedef void(*pReadNode)(void*, _vector_ptr, void*, void*, void*, int);
	Hook *hck_ReadNode;

	typedef void(*pReadBlueprintList)(int, _vector_ptr, void*, void*, void*, int, void*, int);
	Hook *hck_ReadBlueprintList;

	longlong get_sm_handle() {
		return (longlong)GetModuleHandleA("ScrapMechanic.exe");
	}
	void* get_global_tile_editor() {
		return (void*)*(longlong*)(get_sm_handle() + offset_GLOBAL_TILE_EDITOR);
	}

	void AddEntityToMemory(_vector_ptr vec) {
		typedef void(*add_entity)(void*, void*, int);

		void* GLOBAL_TILE_EDITOR = get_global_tile_editor();
		void* field_astruct_entities_0xf8 = *(void**)((longlong)GLOBAL_TILE_EDITOR + 0xf8);

		int value = ((int)(vec[1] - vec[0]) >> 5) * -0x33333333;
		((add_entity)(get_sm_handle() + offset_ADD_ENTITY))(field_astruct_entities_0xf8, (void*)vec[0], value);
	}

	void hook_ReadNode(void* a, _vector_ptr vec, void* b, void* c, void* d, int e) {
		Console::log(Color::Aqua, "hook_ReadNode: a=[%p], vec=[%p], b=[%p], c=[%p], d=[%p], e=[%d]", a, vec, b, c, d, e);
		((pReadNode)hck_ReadNode->Gate())(a, vec, b, c, d, e);

		AddEntityToMemory(vec);
		return;
	}

	void hook_ReadBlueprintList(int a, _vector_ptr vec, void* b, void* c, void* d, int e, void* f, int g) {
		Console::log(Color::Aqua, "hook_ReadBlueprintList: a=[%d], vec=[%p], b=[%p], c=[%p], d=[%p], e=[%d], f=[%p], g=[%d]", a, vec, b, c, d, e, f, g);
		((pReadBlueprintList)hck_ReadBlueprintList->Gate())(a, vec, b, c, d, e, f, g);

		AddEntityToMemory(vec);
		return;
	}
}


LIB_RESULT PluginLoad() {
	Console::log(Color::Aqua, "Unlocking the tile editor!");

	longlong sm_handle = Injection::get_sm_handle();

	Injection::hck_ReadNode = new Hook();
	Injection::hck_ReadNode->Inject((void*)(sm_handle + offset_ReadNode), &Injection::hook_ReadNode, 14);
	
	Injection::hck_ReadBlueprintList = new Hook();
	Injection::hck_ReadBlueprintList->Inject((void*)(sm_handle + offset_ReadBlueprintList), &Injection::hook_ReadBlueprintList, 16);

	return PLUGIN_SUCCESSFULL;
}

LIB_RESULT PluginUnload() {
	Console::log(Color::Red, "Unloading this plugin!");

	Injection::hck_ReadNode->Uninject();
	Injection::hck_ReadBlueprintList->Uninject();

	return PLUGIN_SUCCESSFULL;
}
