#include "main.h"

ssize_t monday(const std::vector<int>& array, int target) {
    ssize_t l = 0;
    ssize_t r = array.size() - 1;

    while (l <= r) {
        ssize_t m = (l + r) / 2;

        if (array[m] == target) {
            return m;
        }

        if (array[m] < target) {
            l = m + 1;
        } else {
            r = m - 1;
        }
    }

    return -1;
}