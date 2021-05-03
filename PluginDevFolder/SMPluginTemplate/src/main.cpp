#define _SM_LIBRARY_BUILD_PLUGIN
#define _SM_PLUGIN_NAME PluginTemplate

#include <sm_lib.h>
#include <console.h>
using Console::Color;

LIB_RESULT PluginLoad() {
	Console::log(Color::Aqua, "Loading this plugin!");
	Console::log(Color::Aqua, "Hello World!");

	return PLUGIN_SUCCESSFULL;
}

LIB_RESULT PluginUnload() {
	Console::log(Color::Aqua, "Unloading this plugin!");
	return PLUGIN_SUCCESSFULL;
}
