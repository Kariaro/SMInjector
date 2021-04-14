#include <windows.h>
#include <string.h>
#include <stdio.h>

typedef unsigned char BYTE;
typedef long long longlong;

class Hook {
	private:
		BYTE *gate = NULL;
		void *func = NULL;
		int length = 0;
		int offset = 0;
	public:
		~Hook();
		bool InjectFromName(const char *module_name, const char *proc_name, void *src, int len);
		bool InjectFromName(const char *module_name, const char *proc_name, void *src, int offset, int len);
		bool Inject(void *dst, void *src, int len);
		bool Inject(void *dst, void *src, int offset, int len);
		bool Uninject();
		void* Gate();
};