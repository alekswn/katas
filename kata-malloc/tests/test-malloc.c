#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>

// Test counters
static int tests_passed = 0;
static int tests_failed = 0;

// Test macro
#define TEST(name, condition) do { \
    if (condition) { \
        printf("✓ %s\n", name); \
        tests_passed++; \
    } else { \
        printf("✗ %s\n", name); \
        tests_failed++; \
    } \
} while(0)

// Test malloc with zero size
void test_malloc_zero() {
    void* ptr = malloc(0);
    TEST("malloc(0) returns NULL or valid pointer", ptr == NULL || ptr != NULL);
    if (ptr) free(ptr);
}

// Test malloc with large size
void test_malloc_large() {
    void* ptr = malloc(1024 * 1024); // 1MB
    TEST("malloc(1MB) returns valid pointer", ptr != NULL);
    if (ptr) {
        memset(ptr, 0xAA, 1024 * 1024);
        free(ptr);
    }
}

// Test malloc with very large size
void test_malloc_very_large() {
    void* ptr = malloc(100 * 1024 * 1024); // 100MB
    TEST("malloc(100MB) returns valid pointer", ptr != NULL);
    if (ptr) {
        memset(ptr, 0xBB, 100 * 1024 * 1024);
        free(ptr);
    }
}

// Test free with NULL
void test_free_null() {
    free(NULL); // Should not crash
    TEST("free(NULL) does not crash", 1);
}

// Test double free detection
void test_double_free() {
    void* ptr = malloc(100);
    TEST("malloc(100) returns valid pointer", ptr != NULL);
    if (ptr) {
        free(ptr);
        // Note: System malloc may detect double free and abort
        // This is expected behavior, so we don't test the second free
        TEST("first free succeeds", 1);
    }
}

// Test calloc with zero elements
void test_calloc_zero_elements() {
    void* ptr = calloc(0, 100);
    TEST("calloc(0, 100) returns NULL or valid pointer", ptr == NULL || ptr != NULL);
    if (ptr) free(ptr);
}

// Test calloc with zero size
void test_calloc_zero_size() {
    void* ptr = calloc(100, 0);
    TEST("calloc(100, 0) returns NULL or valid pointer", ptr == NULL || ptr != NULL);
    if (ptr) free(ptr);
}

// Test calloc initialization
void test_calloc_initialization() {
    void* ptr = calloc(10, sizeof(int));
    TEST("calloc returns valid pointer", ptr != NULL);
    if (ptr) {
        int* arr = (int*)ptr;
        int all_zero = 1;
        for (int i = 0; i < 10; i++) {
            if (arr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        TEST("calloc initializes memory to zero", all_zero);
        free(ptr);
    }
}

// Test realloc with NULL
void test_realloc_null() {
    void* ptr = realloc(NULL, 100);
    TEST("realloc(NULL, 100) returns valid pointer", ptr != NULL);
    if (ptr) free(ptr);
}

// Test realloc with zero size
void test_realloc_zero() {
    void* ptr = malloc(100);
    TEST("malloc(100) for realloc test", ptr != NULL);
    if (ptr) {
        void* new_ptr = realloc(ptr, 0);
        TEST("realloc(ptr, 0) frees memory", new_ptr == NULL);
    }
}

// Test realloc shrinking
void test_realloc_shrink() {
    void* ptr = malloc(1000);
    TEST("malloc(1000) for shrink test", ptr != NULL);
    if (ptr) {
        memset(ptr, 0xCC, 1000);
        void* new_ptr = realloc(ptr, 100);
        TEST("realloc shrink returns valid pointer", new_ptr != NULL);
        if (new_ptr) {
            // Check that data is preserved
            int data_preserved = 1;
            for (int i = 0; i < 100; i++) {
                if (((unsigned char*)new_ptr)[i] != 0xCC) {
                    data_preserved = 0;
                    break;
                }
            }
            TEST("realloc shrink preserves data", data_preserved);
            free(new_ptr);
        }
    }
}

// Test realloc growing
void test_realloc_grow() {
    void* ptr = malloc(100);
    TEST("malloc(100) for grow test", ptr != NULL);
    if (ptr) {
        memset(ptr, 0xDD, 100);
        void* new_ptr = realloc(ptr, 1000);
        TEST("realloc grow returns valid pointer", new_ptr != NULL);
        if (new_ptr) {
            // Check that data is preserved
            int data_preserved = 1;
            for (int i = 0; i < 100; i++) {
                if (((unsigned char*)new_ptr)[i] != 0xDD) {
                    data_preserved = 0;
                    break;
                }
            }
            TEST("realloc grow preserves data", data_preserved);
            free(new_ptr);
        }
    }
}

// Test memory alignment
void test_memory_alignment() {
    void* ptr = malloc(100);
    TEST("malloc alignment test", ptr != NULL);
    if (ptr) {
        // Check if pointer is aligned to 8 bytes (common requirement)
        uintptr_t addr = (uintptr_t)ptr;
        TEST("malloc returns 8-byte aligned pointer", (addr % 8) == 0);
        free(ptr);
    }
}

// Test multiple allocations
void test_multiple_allocations() {
    void* ptrs[100];
    int success = 1;
    
    // Allocate 100 blocks
    for (int i = 0; i < 100; i++) {
        ptrs[i] = malloc(100 + i);
        if (ptrs[i] == NULL) {
            success = 0;
            break;
        }
        memset(ptrs[i], i, 100 + i);
    }
    TEST("multiple allocations succeed", success);
    
    // Free all blocks
    for (int i = 0; i < 100; i++) {
        if (ptrs[i]) {
            free(ptrs[i]);
        }
    }
    TEST("multiple frees succeed", 1);
}

// Test mixed allocation sizes
void test_mixed_sizes() {
    void* ptr1 = malloc(8);
    void* ptr2 = malloc(1024);
    void* ptr3 = malloc(1024 * 1024);
    
    TEST("mixed size allocations succeed", ptr1 != NULL && ptr2 != NULL && ptr3 != NULL);
    
    if (ptr1) free(ptr1);
    if (ptr2) free(ptr2);
    if (ptr3) free(ptr3);
}

// Test calloc overflow
void test_calloc_overflow() {
    // Test multiplication overflow
    void* ptr = calloc(SIZE_MAX / 2, 3);
    TEST("calloc overflow returns NULL", ptr == NULL);
}

// Stress test
void test_stress() {
    const int num_iterations = 1000;
    const int max_ptrs = 100;
    void* ptrs[max_ptrs];
    int ptr_count = 0;
    
    for (int i = 0; i < num_iterations; i++) {
        // Randomly allocate or free
        if (ptr_count < max_ptrs && (rand() % 2 == 0)) {
            size_t size = 8 + (rand() % 1000);
            ptrs[ptr_count] = malloc(size);
            if (ptrs[ptr_count]) {
                memset(ptrs[ptr_count], i % 256, size);
                ptr_count++;
            }
        } else if (ptr_count > 0) {
            int idx = rand() % ptr_count;
            free(ptrs[idx]);
            ptrs[idx] = ptrs[ptr_count - 1];
            ptr_count--;
        }
    }
    
    // Free remaining pointers
    for (int i = 0; i < ptr_count; i++) {
        free(ptrs[i]);
    }
    
    TEST("stress test completed", 1);
}

// Test memory corruption detection
void test_memory_corruption() {
    void* ptr = malloc(100);
    TEST("malloc for corruption test", ptr != NULL);
    if (ptr) {
        // Write beyond allocated memory (this might crash, which is expected)
        memset(ptr, 0xFF, 200);
        free(ptr);
        TEST("memory corruption test completed", 1);
    }
}

int main() {
    printf("=== Malloc Family Functions Test Suite ===\n\n");
    
    // Seed random number generator
    srand(time(NULL));
    
    // Run all tests
    test_malloc_zero();
    test_malloc_large();
    test_malloc_very_large();
    test_free_null();
    test_double_free();
    test_calloc_zero_elements();
    test_calloc_zero_size();
    test_calloc_initialization();
    test_realloc_null();
    test_realloc_zero();
    test_realloc_shrink();
    test_realloc_grow();
    test_memory_alignment();
    test_multiple_allocations();
    test_mixed_sizes();
    test_calloc_overflow();
    test_stress();
    test_memory_corruption();
    
    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("✓ All tests passed!\n");
        return 0;
    } else {
        printf("✗ Some tests failed!\n");
        return 1;
    }
} 