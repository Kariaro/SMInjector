#include "stdafx.h"

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#ifndef _SM_LIBRARY_BUILD_PLUGIN
#include <windows.h>
#include <iostream>
#endif

namespace Console {
	enum class Color {
		Black,
		Blue,
		Green,
		Aqua,
		Red,
		Purple,
		Yellow,
		White,
		Gray,
		LightBlue,
		LightGreen,
		LightAqua,
		LightRed,
		LightPurple,
		LightYellow,
		BrightWhite
	};
	
#ifdef _SM_LIBRARY_BUILD_PLUGIN
	_LIB_IMPORT
	void log(Color color, const char *format, ...);
#else
	FILE* console_handle = NULL;
	HANDLE hConsole;

	void log_close() {
		if(console_handle) {
			fclose(console_handle);
			console_handle = NULL;
		}
	}

	void log_open() {
		log_close();
		freopen_s(&console_handle, "CONOUT$", "w", stdout);
	}

	_LIB_EXPORT
	void log(Color color, const char *format, ...) {
		if(!console_handle) return;

		if(!hConsole) hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		if(hConsole) SetConsoleTextAttribute(hConsole, (WORD)color);

		va_list args;
		va_start(args, format);
		
		vprintf(format, args);
		printf("\n");
	}
#endif
}

#endif
