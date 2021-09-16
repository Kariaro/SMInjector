#define _SM_LIBRARY_ALLOW_OLD_HOOKS
#include "../include/hook.h"

Hook::~Hook() {
	if(gate) {
		Uninject();
	}
	
	gate = NULL;
	func = NULL;
	length = 0;
	offset = 0;
}

bool Hook::InjectFromName(const char *module_name, const char *proc_name, void *src, int len) {
	return Hook::InjectFromName(module_name, proc_name, src, 0, len);
}

bool Hook::InjectFromName(const char *module_name, const char *proc_name, void *src, int offset, int len) {
	HMODULE hModule = GetModuleHandleA(module_name);
	if(!hModule) {
		printf("The module '%s' was not found\n", module_name);
		return false;
	}
	
	void *dst = GetProcAddress(hModule, proc_name);
	if(!dst) {
		printf("The function '%s' was not found in module '%s'\n", proc_name, module_name);
		return false;
	}
	
	return Inject(dst, src, offset, len);
}

bool Hook::Inject(void *dst, void *src, int len) {
	return Hook::Inject(dst, src, 0, len);
}

bool Hook::Inject(void *dst, void *src, int off, int len) {
	if(gate) {
		printf("Already injected!\n");
		return false;
	}

	if(len < 6) return false;
	
	// Construct the gate
	gate = (BYTE*)VirtualAlloc(NULL, (size_t)len + 14, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(!gate) {
		printf("Out of memory!\n");
		return false;
	}
	
	// printf("1: dst=%p\n2: src=%p\n3: gate=%p\n", dst, src, gate);

	memcpy(gate, dst, len); // Save the first bytes
	*(longlong*)((longlong)gate + len) = 0x25FF; // Add far jump
	*(longlong*)((longlong)gate + len + 6) = (longlong)dst + len;
	
	DWORD oldProtection;
	// Update the protection for the gate memory
	VirtualProtect(gate, (size_t)len + 6, PAGE_EXECUTE_READ, &oldProtection);
	
	// Allow modifications of the target function
	VirtualProtect(dst, (size_t)len, PAGE_EXECUTE_READWRITE, &oldProtection);
	
	// Keep the old value where the pointer was placed
	last_pointer = *(longlong*)((longlong)dst + 6 + off);

	memset(dst, 0x90, len); // Fill the replaced region with NOP

	// Modify the target function
	*(short*)((longlong)dst) = 0x25FF; // Add far jump
	*(int*)((longlong)dst + 2) = off;  // Set pointer offset position
	*(longlong*)((longlong)dst + 6 + off) = (longlong)src;
	
	DWORD temp;
	VirtualProtect(dst, (size_t)len, oldProtection, &temp);
	
	// Save references to the our parameters
	offset = off;
	length = len;
	func = dst;
	
	return true;
}

bool Hook::Uninject() {
	if(!gate) {
		printf("Not injected!\n");
		return false;
	}
	
	DWORD oldProtection;
	// Allow modifications of the target function
	VirtualProtect(func, length, PAGE_EXECUTE_READWRITE, &oldProtection);
	
	// Write the old bytes back to the function
	memcpy(func, gate, length);
	
	// Free the gate
	VirtualFree(gate, 0, MEM_RELEASE);
	
	*(longlong*)((longlong)func + 6 + offset) = last_pointer;

	DWORD temp;
	// Change back the protation of the remote function
	VirtualProtect(func, length, oldProtection, &temp);
	
	// Reset values
	gate = NULL;
	func = NULL;
	last_pointer = 0;
	length = 0;
	offset = 0;
	
	return true;
}

inline void *Hook::Gate() {
	/*if(!gate) {
		printf("Invalid gate!\n");
	}*/
	
	return gate;
}
