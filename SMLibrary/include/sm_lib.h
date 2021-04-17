/*
 * Created by HardCoded 2021 (c)
 * 
 * To build a custom plugin add this before you include this file
 *   #define _SM_LIBRARY_BUILD_PLUGIN
 * 
 * 
 * Contributors:
 *     TechnologicNick          (https://github.com/TechnologicNick)
 * 
 * https://github.com/Kariaro/SMInjector
 * 
 */

#include "stdafx.h"

#ifdef _SM_LIBRARY_BUILD_PLUGIN

#include "plugin.h"

int __stdcall DllMain(void *hModule, unsigned long fdwReason, void *lpReserved) {
	if(fdwReason == 1 /* DLL_PROCESS_ATTACH */) {
		PluginStart();
	}

	if(fdwReason == 0 /* DLL_PROCESS_DETACH */) {
		PluginClose();
	}

	return true;
}

#endif
