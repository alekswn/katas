#define _GNU_SOURCE
#include <stddef.h>
#include <sys/mman.h>

#define unlikely(x) __builtin_expect(!!(x), 0)

static const size_t header_size = sizeof(size_t);

void* malloc(size_t size) {
    void* ptr = mmap(0, size+header_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (unlikely(ptr == MAP_FAILED)) {
        return NULL;
    }
    *(size_t*)ptr = size;
    return (void*)((char*)ptr + header_size);
}

void free(void* data_ptr) {
    if (unlikely(data_ptr == NULL)) {
        return;
    }
    void* header_ptr = (char*)data_ptr - header_size;
    const size_t size = *(size_t*)header_ptr;
    munmap(header_ptr, size + header_size);
    //ignore return value
}

void* calloc(size_t nmemb, size_t size) {
    return malloc(nmemb * size);
    //mmap zeros the memory already
}

void* realloc(void* old_data_ptr, size_t new_size) {
    if (unlikely(old_data_ptr == NULL)) {
        return malloc(new_size);
    }
    if (unlikely(new_size == 0)) {
        free(old_data_ptr);
        return NULL;
    }
    void* old_header_ptr = (char*)old_data_ptr - header_size;
    const size_t old_size = *(size_t*)old_header_ptr;
    void* new_ptr = mremap(old_header_ptr, old_size + header_size, new_size + header_size, MREMAP_MAYMOVE);
    if (unlikely(new_ptr == MAP_FAILED)) {
        return NULL;    
    }
    *(size_t*)new_ptr = new_size;
    return (void*)((char*)new_ptr + header_size);
}
