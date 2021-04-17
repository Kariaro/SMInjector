#define _SM_LIBRARY_BUILD_PLUGIN
#include <sm_lib.h>
#include <console.h>
#include <hook.h>


LIB_RESULT PluginStart() {
	Console::log(Console::Color::Red, "Testing this plugin!");

	return PLUGIN_SUCCESSFULL;
}

LIB_RESULT PluginClose() {

	return PLUGIN_SUCCESSFULL;
}