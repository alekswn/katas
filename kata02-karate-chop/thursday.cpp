#include "main.h"

ssize_t thursday(const std::vector<int>& array, int target) {
    ssize_t left = 0;
    ssize_t right = array.size() - 1;

    while (left <= right) {
        ssize_t mid1 = left + (right - left) / 3;
        ssize_t mid2 = right - (right - left) / 3;

        if (array[mid1] == target) {
            return mid1;
        }
        if (array[mid2] == target) {
            return mid2;
        }

        if (target < array[mid1]) {
            right = mid1 - 1;
        } else if (target > array[mid2]) {
            left = mid2 + 1;
        } else {
            left = mid1 + 1;
            right = mid2 - 1;
        }
    }

    return -1;
}
