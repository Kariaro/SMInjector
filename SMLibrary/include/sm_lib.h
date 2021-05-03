/*
 * Created by HardCoded 2021 (c)
 * 
 * To build a custom plugin add this before you include this file
 *   #define _SM_LIBRARY_BUILD_PLUGIN
 *   #define _SM_PLUGIN_NAME PluginName
 * 
 * 
 * Contributors:
 *     TechnologicNick          (https://github.com/TechnologicNick)
 * 
 * https://github.com/Kariaro/SMInjector
 * 
 */

#include "stdafx.h"

typedef int LIB_RESULT;
typedef LIB_RESULT (*LIB_CALLBACK)();

#ifndef _SM_PLUGIN_NAME
#   error _SM_PLUGIN_NAME was undefined
#endif

#define _LIB_PLUGIN_NAME_STR__(X) #X
#define _LIB_PLUGIN_NAME_STR_(X) _LIB_PLUGIN_NAME_STR__(X)
#define _LIB_PLUGIN_NAME_STR "" _LIB_PLUGIN_NAME_STR_(_SM_PLUGIN_NAME) ""

#ifdef _SM_LIBRARY_BUILD_PLUGIN
extern LIB_RESULT PluginLoad();
extern LIB_RESULT PluginUnload();

_LIB_IMPORT void InjectPlugin(void*, const char*, LIB_CALLBACK, LIB_CALLBACK);

int __stdcall DllMain(void* hModule, unsigned long fdwReason, void* lpReserved) {
	if(fdwReason == 1 /* DLL_PROCESS_ATTACH */) {
		InjectPlugin(hModule, _LIB_PLUGIN_NAME_STR, PluginLoad, PluginUnload);
	}

	//if(fdwReason == 0 /* DLL_PROCESS_DETACH */) {
	//	UnloadLibrary(hModule, PluginUnload);
	//}

	return true;
}
#else
extern _LIB_EXPORT void InjectPlugin(void*, const char*, LIB_CALLBACK, LIB_CALLBACK);
#endif
