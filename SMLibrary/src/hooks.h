#include <stdio.h>
#include "../include/hook.h"

#include "../include/console.h"
using Console::Color;

// INIT_CONSOLE
typedef void (*pinit_console)(void*, void*);
Hook *hck_init_console;

// =============

namespace Hooks {
	void hook_init_console(void* a, void* b) {
		FreeConsole();
		((pinit_console)hck_init_console->Gate())(a, b);
		PostConsoleInjections();
		return;
	}
}
