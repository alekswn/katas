#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>

// Default values
#define DEFAULT_NUM_ALLOCATIONS (100000)
#define DEFAULT_MAX_ALLOC_SIZE (4ULL * 1024ULL * 1024ULL * 1024ULL)  // 4GB
#define MIN_ALLOC_SIZE (8)
#define NUM_SIZE_BINS (5)
#define PERCENTILE_SAMPLE_SIZE (1000) // Fixed size for percentile samples

// Fixed bucket boundaries
#define BUCKET_4K    (4ULL * 1024ULL)
#define BUCKET_2MB   (2ULL * 1024ULL * 1024ULL)
#define BUCKET_100MB (100ULL * 1024ULL * 1024ULL)
#define BUCKET_1GB   (1024ULL * 1024ULL * 1024ULL)

// Global configuration
static int num_allocations = DEFAULT_NUM_ALLOCATIONS;
static size_t max_alloc_size = DEFAULT_MAX_ALLOC_SIZE;
static int distribution_type = 2; // 0=uniform, 1=weighted, 2=exponential
static int disable_memset = 1; // 0=enabled, 1=disabled

// Histogram structure based on size
typedef struct {
    long long min_latency;
    long long max_latency;
    long long total_latency;
    int total_operations;
    size_t size_range_start;
    size_t size_range_end;
    long long percentile_samples[PERCENTILE_SAMPLE_SIZE]; // Fixed-size array for sampling
    int sample_count; // Number of samples collected
} size_bucket_t;

// Size-based histogram for each function
typedef struct {
    size_bucket_t buckets[NUM_SIZE_BINS];
} size_histogram_t;

// Global histograms for each function
size_histogram_t histograms[4]; // 4 functions: malloc, calloc, realloc, free

// Get current time in nanoseconds using monotonic clock
long long get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

// Get current time in microseconds using monotonic clock
long long get_time_us(void) {
    return get_time_ns() / 1000LL;
}

// Clean up histogram memory
void cleanup_histogram(size_histogram_t* hist) {
    for (int i = 0; i < NUM_SIZE_BINS; i++) {
        hist->buckets[i].sample_count = 0;
    }
}

// Initialize size-based histogram with fixed buckets
void init_size_histogram(size_histogram_t* hist) {
    // Define fixed bucket boundaries
    size_t boundaries[] = {0, BUCKET_4K, BUCKET_2MB, BUCKET_100MB, BUCKET_1GB, max_alloc_size};
    
    for (int i = 0; i < NUM_SIZE_BINS; i++) {
        hist->buckets[i].min_latency = LLONG_MAX;
        hist->buckets[i].max_latency = 0;
        hist->buckets[i].total_latency = 0;
        hist->buckets[i].total_operations = 0;
        hist->buckets[i].size_range_start = boundaries[i];
        hist->buckets[i].size_range_end = boundaries[i + 1];
        hist->buckets[i].sample_count = 0;
    }
}

// Get size bucket index for fixed buckets
int get_size_bucket(size_t size) {
    if (size < BUCKET_4K) return 0;
    if (size < BUCKET_2MB) return 1;
    if (size < BUCKET_100MB) return 2;
    if (size < BUCKET_1GB) return 3;
    return 4; // > 1GB bucket
}

// Add latency measurement to size-based histogram
void add_latency_to_size_bucket(size_histogram_t* hist, size_t size, long long latency_ns) {
    int bucket = get_size_bucket(size);
    size_bucket_t* b = &hist->buckets[bucket];
    
    if (latency_ns < b->min_latency) b->min_latency = latency_ns;
    if (latency_ns > b->max_latency) b->max_latency = latency_ns;
    
    b->total_latency += latency_ns;
    b->total_operations++;
    
    // Reservoir sampling for percentile calculation
    if (b->sample_count < PERCENTILE_SAMPLE_SIZE) {
        // Fill the sample array initially
        b->percentile_samples[b->sample_count] = latency_ns;
        b->sample_count++;
    } else {
        // Reservoir sampling: replace with probability PERCENTILE_SAMPLE_SIZE/total_operations
        int j = rand() % b->total_operations;
        if (j < PERCENTILE_SAMPLE_SIZE) {
            b->percentile_samples[j] = latency_ns;
        }
    }
}

// Compare function for qsort
int compare_latencies(const void* a, const void* b) {
    return (*(long long*)a - *(long long*)b);
}

// Calculate percentile from sample array
long long calculate_percentile(const long long* samples, int count, double percentile) {
    if (count == 0) return 0;
    
    // Create a temporary array for sorting (since we don't want to modify the original)
    long long temp_samples[PERCENTILE_SAMPLE_SIZE];
    memcpy(temp_samples, samples, count * sizeof(long long));
    
    // Sort the temporary array
    qsort(temp_samples, count, sizeof(long long), compare_latencies);
    
    // Calculate index for percentile
    double index = (percentile / 100.0) * (count - 1);
    int lower_index = (int)index;
    int upper_index = lower_index + 1;
    
    if (upper_index >= count) {
        return temp_samples[count - 1];
    }
    
    // Linear interpolation
    double fraction = index - lower_index;
    return (long long)(temp_samples[lower_index] * (1 - fraction) + temp_samples[upper_index] * fraction);
}

// Print size-based histogram
void print_size_histogram(const char* function_name, const size_histogram_t* hist) {
    printf("\n%s - Latency by Allocation Size:\n", function_name);
    printf("Size Range        | Operations |  Min (ns)  |  Max (ns)  |  Avg (ns)  |  P50 (ns)  |  P90 (ns)  |  P99 (ns)  | Distribution\n");
    printf("------------------|------------|------------|------------|------------|------------|------------|------------|-------------\n");
    
    const char* bucket_names[] = {"4K", "2MB", "100MB", "1GB", ">1GB"};
    
    for (int i = 0; i < NUM_SIZE_BINS; i++) {
        const size_bucket_t* bucket = &hist->buckets[i];
        if (bucket->total_operations == 0) continue;
        
        double avg_latency = (double)bucket->total_latency / bucket->total_operations;
        
        // Calculate percentiles
        long long p50 = calculate_percentile(bucket->percentile_samples, bucket->sample_count, 50.0);
        long long p90 = calculate_percentile(bucket->percentile_samples, bucket->sample_count, 90.0);
        long long p99 = calculate_percentile(bucket->percentile_samples, bucket->sample_count, 99.0);
        
        printf("%-17s | %10d | %10lld | %10lld | %10.0f | %10lld | %10lld | %10lld | ", 
               bucket_names[i],
               bucket->total_operations, bucket->min_latency, bucket->max_latency, avg_latency,
               p50, p90, p99);
        
        // Print simple bar chart
        int bar_length = (bucket->total_operations * 20) / num_allocations;
        for (int j = 0; j < bar_length; j++) {
            printf("#");
        }
        printf("\n");
    }
}

// Weighted random size generation to favor smaller allocations
size_t generate_weighted_size(void) {
    // Use a weighted distribution: 73% small, 20% medium, 5% large, 2% huge
    int rand_val = rand() % 100;
    
    if (rand_val < 73) {
        // 73% chance: small allocations (8 bytes to 4KB)
        return MIN_ALLOC_SIZE + (rand() % (BUCKET_4K - MIN_ALLOC_SIZE));
    } else if (rand_val < 93) {
        // 20% chance: medium allocations (4KB to 2MB)
        return BUCKET_4K + (rand() % (BUCKET_2MB - BUCKET_4K));
    } else if (rand_val < 98) {
        // 5% chance: large allocations (2MB to 1GB)
        return BUCKET_2MB + (rand() % (BUCKET_1GB - BUCKET_2MB));
    } else {
        // 2% chance: huge allocations (1GB to max_alloc_size)
        return BUCKET_1GB + (rand() % (max_alloc_size - BUCKET_1GB));
    }
}

// Alternative: exponential distribution favoring smaller sizes
size_t generate_exponential_size(void) {
    // Use exponential distribution to favor smaller sizes
    double u = (double)rand() / RAND_MAX;
    double log_size = log(MIN_ALLOC_SIZE) + u * log(max_alloc_size / MIN_ALLOC_SIZE);
    size_t size = (size_t)exp(log_size);
    
    // Ensure size is within bounds
    if (size < MIN_ALLOC_SIZE) size = MIN_ALLOC_SIZE;
    if (size > max_alloc_size) size = max_alloc_size;
    
    return size;
}

// Generate allocation size based on selected distribution
size_t generate_allocation_size(void) {
    switch (distribution_type) {
        case 1:
            return generate_weighted_size();
        case 2:
            return generate_exponential_size();
        default:
            // Uniform distribution (original)
            return MIN_ALLOC_SIZE + (rand() % (max_alloc_size - MIN_ALLOC_SIZE));
    }
}

// Benchmark: Sequential allocations and frees with size-based histogram
void benchmark_sequential_alloc_free(void) {
    printf("Benchmark: Sequential allocation and free\n");
    
    void** ptrs = malloc(num_allocations * sizeof(void*));
    size_t* sizes = malloc(num_allocations * sizeof(size_t)); // Track actual sizes
    if (!ptrs || !sizes) {
        printf("Failed to allocate arrays\n");
        free(ptrs);
        free(sizes);
        return;
    }
    
    // Initialize histograms
    init_size_histogram(&histograms[0]); // malloc
    init_size_histogram(&histograms[3]); // free
    
    // Allocate memory sequentially
    for (int i = 0; i < num_allocations; i++) {
        size_t size = generate_allocation_size();
        sizes[i] = size; // Store the actual size
        
        long long start_time = get_time_ns();
        ptrs[i] = malloc(size);
        long long end_time = get_time_ns();
        
        if (ptrs[i]) {
            if (!disable_memset) {
                memset(ptrs[i], i % 256, size);
            }
            add_latency_to_size_bucket(&histograms[0], size, end_time - start_time);
        }
    }
    
    // Free memory sequentially
    for (int i = 0; i < num_allocations; i++) {
        if (ptrs[i]) {
            long long start_time = get_time_ns();
            free(ptrs[i]);
            long long end_time = get_time_ns();
            
            add_latency_to_size_bucket(&histograms[3], sizes[i], end_time - start_time);
        }
    }
    
    free(ptrs);
    free(sizes);
}

// Benchmark: Calloc operations with size-based histogram
void benchmark_calloc(void) {
    printf("Benchmark: Calloc operations\n");
    
    void** ptrs = malloc(num_allocations * sizeof(void*));
    if (!ptrs) {
        printf("Failed to allocate pointer array\n");
        return;
    }
    
    // Initialize histogram
    init_size_histogram(&histograms[1]); // calloc
    
    for (int i = 0; i < num_allocations; i++) {
        size_t total_size = generate_allocation_size();
        
        // For calloc, we need to split total_size into nmemb * size
        // Let's use a reasonable split to simulate realistic calloc usage
        size_t size = 1 + (rand() % 16); // Element size 1-16 bytes
        size_t nmemb = total_size / size;
        if (nmemb == 0) nmemb = 1;
        
        long long start_time = get_time_ns();
        ptrs[i] = calloc(nmemb, size);
        long long end_time = get_time_ns();
        
        if (ptrs[i]) {
            add_latency_to_size_bucket(&histograms[1], total_size, end_time - start_time);
        }
    }
    
    for (int i = 0; i < num_allocations; i++) {
        free(ptrs[i]);
    }
    
    free(ptrs);
}

// Benchmark: Realloc operations with size-based histogram
void benchmark_realloc(void) {
    printf("Benchmark: Realloc operations\n");
    
    // Start with a reasonable initial size
    size_t initial_size = 1024; // 1KB
    void* ptr = malloc(initial_size);
    if (ptr) {
        memset(ptr, 0xAA, initial_size);
        
        // Initialize histogram
        init_size_histogram(&histograms[2]); // realloc
        
        for (int i = 0; i < num_allocations; i++) {
            size_t new_size = generate_allocation_size();
            
            long long realloc_start = get_time_ns();
            void* new_ptr = realloc(ptr, new_size);
            long long realloc_end = get_time_ns();
            
            if (new_ptr) {
                ptr = new_ptr;
                if (!disable_memset) {
                    memset(ptr, i % 256, new_size);
                }
                add_latency_to_size_bucket(&histograms[2], new_size, realloc_end - realloc_start);
            }
        }
        
        free(ptr);
    }
}

// Print all size-based histograms
void print_all_size_histograms(void) {
    const char* function_names[] = {"malloc", "calloc", "realloc", "free"};
    
    printf("\n=== SIZE-BASED LATENCY HISTOGRAMS ===\n");
    
    for (int func = 0; func < 4; func++) {
        print_size_histogram(function_names[func], &histograms[func]);
    }
}

// Parse size string (e.g., "1K", "2M", "1G") to bytes
size_t parse_size_string(const char* str) {
    char* endptr;
    size_t size = strtoull(str, &endptr, 10);
    
    if (endptr == str) {
        fprintf(stderr, "Invalid size format: %s\n", str);
        exit(1);
    }
    
    switch (*endptr) {
        case 'K':
        case 'k':
            size *= 1024;
            break;
        case 'M':
        case 'm':
            size *= 1024 * 1024;
            break;
        case 'G':
        case 'g':
            size *= 1024ULL * 1024ULL * 1024ULL;
            break;
        case '\0':
            break;
        default:
            fprintf(stderr, "Invalid size suffix: %c\n", *endptr);
            exit(1);
    }
    
    return size;
}

void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  -n NUM    Number of allocations per test (default: %d)\n", DEFAULT_NUM_ALLOCATIONS);
    printf("  -s SIZE   Maximum allocation size (default: %llu bytes)\n", DEFAULT_MAX_ALLOC_SIZE);
    printf("            Size can be specified as: 1024, 1K, 2M, 1G, etc.\n");
    printf("  -d TYPE   Distribution type (default: 2)\n");
    printf("            0 = uniform distribution\n");
    printf("            1 = weighted distribution (73%% small, 20%% medium, 5%% large, 2%% huge)\n");
    printf("            2 = exponential distribution (favors smaller sizes)\n");
    printf("  -m        Enable memset calls (default: disabled)\n");
    printf("  -h        Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s                    # Use defaults\n", program_name);
    printf("  %s -n 1000            # 1000 allocations per test\n", program_name);
    printf("  %s -s 1M              # Max size 1MB\n", program_name);
    printf("  %s -d 1               # Use weighted distribution\n", program_name);
    printf("  %s -m                 # Enable memset calls\n", program_name);
    printf("  %s -n 50000 -s 100M -d 0 -m # 50K allocations, max 100MB, uniform dist, with memset\n", program_name);
}

int main(int argc, char* argv[]) {
    int opt;
    
    // Parse command line arguments
    while ((opt = getopt(argc, argv, "n:s:d:mh")) != -1) {
        switch (opt) {
            case 'n':
                num_allocations = atoi(optarg);
                if (num_allocations <= 0) {
                    fprintf(stderr, "Number of allocations must be positive\n");
                    exit(1);
                }
                break;
            case 's':
                max_alloc_size = parse_size_string(optarg);
                if (max_alloc_size < MIN_ALLOC_SIZE) {
                    fprintf(stderr, "Maximum allocation size must be at least %d bytes\n", MIN_ALLOC_SIZE);
                    exit(1);
                }
                break;
            case 'd':
                distribution_type = atoi(optarg);
                if (distribution_type < 0 || distribution_type > 2) {
                    fprintf(stderr, "Distribution type must be 0, 1, or 2\n");
                    exit(1);
                }
                break;
            case 'm':
                disable_memset = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                exit(0);
            default:
                print_usage(argv[0]);
                exit(1);
        }
    }
    
    printf("=== Library Malloc Benchmark ===\n");
    printf("Allocations per test: %d\n", num_allocations);
    printf("Allocation size range: %d - %zu bytes\n", MIN_ALLOC_SIZE, max_alloc_size);
    
    const char* dist_names[] = {"uniform", "weighted", "exponential"};
    printf("Distribution type: %s\n", dist_names[distribution_type]);
    printf("Memset calls: %s\n\n", disable_memset ? "disabled" : "enabled");
    
    // Seed random number generator
    srand(time(NULL));
    
    // Run benchmarks
    benchmark_sequential_alloc_free();
    benchmark_calloc();
    benchmark_realloc();
    
    // Print histograms
    print_all_size_histograms();
    
    // Clean up histogram memory
    for (int i = 0; i < 4; i++) {
        cleanup_histogram(&histograms[i]);
    }
    
    printf("\n=== Benchmark Complete ===\n");
    
    return 0;
} 