#include "main.h"

static ssize_t binary_search(const std::vector<int>& array, int target, ssize_t left, ssize_t right) {
    while (left <= right) {
        ssize_t middle = left + (right - left) / 2;

        if (array[middle] == target) {
            return middle;
        } else if (array[middle] < target) {
            left = middle + 1;
        } else {
            right = middle - 1;
        }
    }
    return -1;
}

ssize_t friday(const std::vector<int>& array, int target) {
    if (array.empty()) {
        return -1;
    }

    ssize_t bound = 1;
    while (bound < std::ssize(array) && array[bound] < target) {
        bound *= 2;
    }

    return binary_search(array, target, bound / 2, std::min(bound, static_cast<ssize_t>(array.size() - 1)));
}
