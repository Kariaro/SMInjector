#include "stdafx.h"

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#ifndef _SM_LIBRARY_BUILD_PLUGIN
#include <windows.h>
#include <iostream>

#ifdef _SM_OUTPUT_LOGS
#include <fcntl.h>
#include <io.h>
#endif

#endif

#include <cstdarg>

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
	_LIB_IMPORT void vlogf(Color color, const char* plugin_name, const char* format, va_list args);
	_LIB_IMPORT void wvlogf(Color color, const char* plugin_name, const wchar_t* format, va_list args);
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
#ifdef _SM_OUTPUT_LOGS
		freopen_s(&console_handle, "CONOUT$", "w", stdout);
#else
		freopen_s(&console_handle, "CONOUT$", "w", stdout);
#endif
	}

	_LIB_EXPORT void vlogf(Color color, const char *plugin_name, const char *format, va_list args) {
		if(!console_handle) return;

		if(!hConsole) hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		if(hConsole) SetConsoleTextAttribute(hConsole, (WORD)color);

		printf("[%s]: ", plugin_name);
		vprintf(format, args);
		printf("\n");
	}

	_LIB_EXPORT void wvlogf(Color color, const char* plugin_name, const wchar_t* format, va_list args) {
		if (!console_handle) return;

		if (!hConsole) hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hConsole) SetConsoleTextAttribute(hConsole, (WORD)color);

		printf("[%s]: ", plugin_name);
		vwprintf(format, args);
		printf("\n");
	}
#endif
	void log(Color color, const char *format, ...) {
		va_list args;
		va_start(args, format);
		vlogf(color, _LIB_PLUGIN_NAME_STR, format, args);
	}

	void wlog(Color color, const wchar_t* format, ...) {
		va_list args;
		va_start(args, format);
		wvlogf(color, _LIB_PLUGIN_NAME_STR, format, args);
	}
}

#endif
