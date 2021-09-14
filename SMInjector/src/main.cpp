#define _DEBUG_

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

#include <iterator>
#include <filesystem>

#include "find_steam.h"

namespace fs = std::filesystem;

bool Inject(HANDLE, const wchar_t*);

inline bool does_file_exist(const wchar_t* name) {
    struct _stat buffer;
    return (_wstat(name, &buffer) == 0);
}

fs::path get_dir_path() {
	TCHAR dir[MAX_PATH] = { 0 };
	DWORD length = GetModuleFileName(NULL, dir, _countof(dir));
	return fs::path(dir).parent_path();
}

BOOL startup(fs::path in_exe, fs::path in_dir, std::wstring in_cmd, HANDLE &hProcess, HANDLE &hThread) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	wchar_t exe[MAX_PATH + 1] = { 0 };
	wchar_t dir[MAX_PATH + 1] = { 0 };
	wchar_t cmd[4097] = { 0 };
	ZeroMemory(cmd, 4097);

	memcpy(exe, in_exe.c_str(), min(in_exe.string().size() * 2, MAX_PATH + 1));
	memcpy(dir, in_dir.c_str(), min(in_dir.string().size() * 2, MAX_PATH + 1));
	swprintf_s(cmd, 4097, L"\"%s\" %s", exe, in_cmd.c_str());

	//wprintf(L"EXE: [%s]\n", exe);
	//wprintf(L"DIR: [%s]\n", dir);
	//wprintf(L"CMD: [%s]\n", cmd);

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	si.wShowWindow = true;

	if(!CreateProcessW(
		exe,
		cmd,
		NULL, NULL,
		FALSE,
		CREATE_SUSPENDED,
		NULL,
		dir,
		&si,
		&pi
	)) {
		printf("Error: [%d]\n", GetLastError());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return false;
	}

	hProcess = pi.hProcess;
	hThread = pi.hThread;
	
	return true;
}

int main(int argc, char** argv) {
	printf("SMInjector: startup\n");
	
	fs::path game_path = SteamFinder::FindGame(L"Scrap Mechanic");
	fs::path dir_path = get_dir_path();
	fs::path dll_path = dir_path / "SMLibrary.dll";

	wprintf(L"GAME: [%s]\n", game_path.c_str());
	wprintf(L"DLL : [%s]\n", dll_path.c_str());
	
	if(game_path.empty()) {
		printf("SMInjector: Failed to find the game \"ScrapMechanic\"\n");
		return 0;
	}
	
	HANDLE hProcess;
	HANDLE hThread;
	fs::path exe_dir = game_path / "Release";
	fs::path exe_exe = game_path / "Release" / "ScrapMechanic.exe";
	if(startup(exe_exe, exe_dir, L"-dev", hProcess, hThread)) {
		if(!Inject(hProcess, dll_path.c_str())) {
			printf("SMInjector: failed to inject dll file\n");
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			return 0;
		}

		if (!fs::exists(dir_path / "plugins" / "dependencies")) {
			fs::create_directories(dir_path / "plugins" / "dependencies");
		}

		printf("\nSMInjector: Adding dependencies...\n");
		for (const auto& file : fs::directory_iterator(dir_path / "plugins" / "dependencies")) {
			if (!Inject(hProcess, file.path().c_str())) {
				wprintf(L"  Failed to inject '%s'\n", file.path().filename().c_str());
				return 0;
			} else {
				wprintf(L"  Injected '%s'\n", file.path().filename().c_str());
			}
		}

		printf("\nSMInjector: Adding plugins...\n");
        for(const auto& file : fs::directory_iterator(get_dir_path() / "plugins")) {
			if(!Inject(hProcess, file.path().c_str())) {
				wprintf(L"  Failed to inject '%s'\n", file.path().filename().c_str());
				return 0;
			} else {
				wprintf(L"  Injected '%s'\n", file.path().filename().c_str());
			}
        }
		
		Sleep(500);
		ResumeThread(hThread);
	}

	return 0;
}

bool Inject(HANDLE hProc, const wchar_t* path) {
	if(!does_file_exist(path)) {
		return false;
	}

	const size_t path_length = wcslen(path) * 2;

	LPVOID lpvoid = VirtualAllocEx(hProc, NULL, path_length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(!lpvoid) {
		return false;
	}

	WriteProcessMemory(hProc, lpvoid, path, path_length, NULL);
	HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
	if(!kernel32) {
		return false;
	}

	LPVOID LoadLibAddr = (LPVOID)GetProcAddress(kernel32, "LoadLibraryW");
	HANDLE threadID = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, lpvoid, NULL, NULL);
	if(!threadID) {
		return false;
	}

	WaitForSingleObject(threadID, INFINITE);
	VirtualFreeEx(hProc, lpvoid, 0, MEM_RELEASE);
	CloseHandle(threadID);
	return true;
}
