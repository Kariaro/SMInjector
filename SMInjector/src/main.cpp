#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include "find_steam.h"

bool inject(DWORD, const char*);
DWORD find_game_pid();

std::string get_dll_path() {
    CHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos).append("\\SMInjectorDll.dll");
}

DWORD startup(std::string in_exe, std::string in_dir, std::string in_cmd, HANDLE &hProcess, HANDLE &hThread) {
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	CHAR exe[MAX_PATH + 1] = { 0 };
	CHAR dir[MAX_PATH + 1] = { 0 };
	CHAR cmd[4097] = { 0 };
	ZeroMemory(cmd, 4097);

	memcpy(exe, in_exe.c_str(), min(in_exe.size(), MAX_PATH + 1));
	memcpy(dir, in_dir.c_str(), min(in_dir.size(), MAX_PATH + 1));
	sprintf_s(cmd, 4097, "\"%s\" %s", exe, in_cmd.c_str());
	printf("EXE: [%s]\n", exe);
	printf("DIR: [%s]\n", dir);
	printf("CMD: [%s]\n", cmd);

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	si.wShowWindow = true;

	// start the program up
	if(!CreateProcessA(
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
		return GetLastError();
	}

	hProcess = pi.hProcess;
	hThread = pi.hThread;
	
	return 0;
}

int main(int argc, char** argv) {
	printf("SMInjector: startup\n");

	std::string dll_path = get_dll_path();
	std::string result = SteamFinder::FindGame("Scrap Mechanic");
	printf("DLL : [%s]\n", dll_path.c_str());
	printf("GAME: [%s]\n", result.c_str());

	if(result.empty()) {
		printf("SMInjector: Failed to find the game \"ScrapMechanic\"\n");
		return 0;
	}
	
	HANDLE hProcess;
	HANDLE hThread;
	std::string exe_exe = std::string(result).append("\\Release\\ScrapMechanic.exe");
	std::string exe_dir = std::string(result).append("\\Release");
	if(startup(exe_exe, exe_dir, "-dev", hProcess, hThread) == ERROR_SUCCESS) {
		printf("hProcess: [%p]\n", hProcess);
		printf("hThread : [%p]\n", hThread);
		
		DWORD pid = find_game_pid();
		printf("PID : [%d]\n", pid);
		
		if(!inject(pid, dll_path.c_str())) {
			printf("SMInjector: failed to inject dll file\n");
			return 0;
		}

		ResumeThread(hThread);
	}

	return 0;
}

DWORD find_game_pid() {
	const wchar_t* procName = L"ScrapMechanic.exe";
	HANDLE hsnap;
	PROCESSENTRY32 pt{};
	hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	do {
		if(!wcscmp(pt.szExeFile, procName)) {
			DWORD pid = pt.th32ProcessID;
			CloseHandle(hsnap);
			return pid;
		}
	} while(Process32Next(hsnap, &pt));
	CloseHandle(hsnap);
	return 0;
} 

bool inject(DWORD pid, const char* path) {
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	if(process) {
		LPVOID lpvoid = VirtualAllocEx(process, NULL, strlen(path), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		if(!lpvoid) {
			CloseHandle(process);
			return false;
		}
		WriteProcessMemory(process, lpvoid, path, strlen(path), NULL);

		HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
		if(!kernel32) {
			CloseHandle(process);
			return false;
		}

		LPVOID LoadLibAddr = (LPVOID)GetProcAddress(kernel32, "LoadLibraryA");
		HANDLE threadID = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, lpvoid, NULL, NULL);
		if(!threadID) {
			CloseHandle(process);
			return false;
		}

		WaitForSingleObject(threadID, INFINITE);
		VirtualFreeEx(process, lpvoid, 0, MEM_RELEASE);
		CloseHandle(threadID);
		CloseHandle(process);

		return true;
	}

	return false;
}
