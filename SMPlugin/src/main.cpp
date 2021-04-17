#define _SM_LIBRARY_BUILD_PLUGIN
#define _SM_PLUGIN_NAME "TestPlugin"

#include <sm_lib.h>
#include <console.h>
using Console::Color;

#include <hook.h>


LIB_RESULT PluginLoad() {
	Console::log(Color::Red, "[TestPlugin]: Testing this plugin!");

	return PLUGIN_SUCCESSFULL;
}

LIB_RESULT PluginUnload() {
	Console::log(Color::Red, "[TestPlugin]: Unloading this plugin!");

	return PLUGIN_SUCCESSFULL;
}