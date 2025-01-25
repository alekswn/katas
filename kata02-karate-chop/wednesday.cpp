#include "main.h"

static 
std::vector<int>::const_iterator iterator_chop(std::vector<int>::const_iterator left,
                                               std::vector<int>::const_iterator right,
                                               int target) {
    while (left != right) {
        auto middle = left + std::distance(left, right) / 2;
        if (*middle == target)
            return middle;
        if (*middle < target)
            left = middle + 1;
        else
            right = middle;
    }
    return std::vector<int>::const_iterator();
}

ssize_t wednesday(const std::vector<int>& array, int target) {
    auto it = iterator_chop(array.cbegin(), array.cend(), target);
    if (it == std::vector<int>::const_iterator()) {
        return -1;
    }
    return std::distance(array.cbegin(), it);
}
