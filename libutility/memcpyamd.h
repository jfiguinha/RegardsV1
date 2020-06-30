#ifndef MEMCPYAMD
#define MEMCPYAMD


void * memcpy_amd(void *dest, const void *src, size_t n);
void * memcpy_mmx(void *dest, const void *src, size_t n);
void * memcpy_sse2(void *dest, const void *src, size_t n);

void* __stdcall memcpy_optimized(void *dest, const void *src, size_t n);

void* __stdcall memset_optimized(void *dest, int c, size_t n);

void __stdcall memzero_optimized(void *dest, size_t n);

unsigned long get_cpu_type();

bool CheckSSE2();

#endif