#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

bool inject(DWORD, const char* path);
DWORD find_game_pid();

int main(int argc, char** argv) {
	printf("SMInjector: startup\n");
	DWORD pid = find_game_pid();

	// TODO: https://stackoverflow.com/questions/875249/how-to-get-current-directory

	if(!inject(pid, "C:\\Users\\Admin\\source\\repos\\SMInjectorProject\\SMInjector\\x64\\Release\\SMInjectorDll.dll")) {
		printf("SMInjector: failed to inject dll file\n");
		return 0;
	}

	printf("SMInjector: sucessfully injected dll file\n");
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
