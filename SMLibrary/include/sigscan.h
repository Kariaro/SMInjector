#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <Psapi.h>

#include "console.h"

#define ERROR_GETMODULE(reason, ...) Console::log(Color::LightRed, "[SignatureScanner] Failed getting handle of module %s. Reason: " reason, moduleName, __VA_ARGS__)

using Console::Color;

class SignatureScanner {
private:
	LPCWSTR moduleName;
	DWORD64 moduleBase;
	DWORD moduleSize;

	MODULEINFO GetModuleInfo(LPCWSTR moduleName) {
		MODULEINFO moduleInfo = { 0 };
		HMODULE hModule = GetModuleHandle(moduleName);
		if (hModule == NULL)
			return moduleInfo;

		GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO));

		return moduleInfo;
	}

public:
	SignatureScanner(LPCWSTR moduleName) {
		this->moduleName = moduleName;

		MODULEINFO moduleInfo = GetModuleInfo(moduleName);

		if (moduleInfo.SizeOfImage == 0) {
			Console::log(Color::LightRed, "[SignatureScanner] Failed getting info of module %s", moduleName);
			return;
		}

		this->moduleBase = (DWORD64) moduleInfo.lpBaseOfDll;
		this->moduleSize = moduleInfo.SizeOfImage;

		Console::wlog(Color::Aqua, L"[SignatureScanner] Found module %s with base=[0x%llX] size=[%ul]", moduleName, this->moduleBase, this->moduleSize);
	}

};

#undef ERROR_GETMODULE
