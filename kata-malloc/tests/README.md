# Malloc Family Functions Test Suite

This directory contains a comprehensive test suite for verifying the correctness of malloc family functions.

## Test Coverage

The test suite covers the following areas:

### Basic Functionality
- `malloc()` with various sizes (0, small, large, very large)
- `free()` with NULL pointer and double free scenarios
- `calloc()` with zero elements, zero size, and overflow conditions
- `realloc()` with NULL, zero size, shrinking, and growing

### Edge Cases
- Memory alignment requirements
- Multiple concurrent allocations
- Mixed allocation sizes
- Memory corruption detection

### Stress Testing
- Random allocation/free patterns
- High-frequency operations
- Memory pressure scenarios

## Building and Running

### Build the test binary
```bash
make
```

### Test with system malloc
```bash
make test-system
```

### Test with naive malloc implementation
```bash
# First build the naive malloc library
cd ../naive-malloc
make

# Then run tests with naive malloc
cd ../tests
make

LD_PRELOAD=../naive-malloc/libnaive-malloc.so ./test-malloc 
```

## Test Output

The test suite provides detailed output showing:
- Individual test results (✓ for pass, ✗ for fail)
- Test summary with pass/fail counts
- Overall success/failure status

Example output:
```
=== Malloc Family Functions Test Suite ===

✓ malloc(0) returns NULL or valid pointer
✓ malloc(1MB) returns valid pointer
✓ malloc(100MB) returns valid pointer
✓ free(NULL) does not crash
✓ double free does not crash
...

=== Test Summary ===
Tests passed: 20
Tests failed: 0
Total tests: 20
✓ All tests passed!
```

## Test Categories

### 1. Basic Allocation Tests
- Zero-size allocations
- Large allocations (1MB, 100MB)
- Memory alignment verification

### 2. Free Tests
- NULL pointer handling
- Double free scenarios
- Memory corruption detection

### 3. Calloc Tests
- Zero element/size handling
- Memory initialization verification
- Overflow detection

### 4. Realloc Tests
- NULL pointer handling
- Zero size (should free)
- Shrinking allocations
- Growing allocations
- Data preservation verification

### 5. Stress Tests
- Random allocation patterns
- High-frequency operations
- Memory pressure scenarios

## Notes

- The test suite is designed to be robust and handle edge cases gracefully
- Some tests may trigger expected behavior (like crashes on memory corruption)
- The naive malloc implementation may have different behavior than system malloc
- All tests should pass with both system and naive malloc implementations

## Troubleshooting

If tests fail with the naive malloc implementation:
1. Check that the naive malloc library is built correctly
2. Verify that `LD_PRELOAD` is working properly
3. Check for any compilation warnings or errors
4. Review the naive malloc implementation for potential issues 