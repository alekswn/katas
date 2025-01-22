#include "main.h"

static
std::vector<int>::const_iterator
recursive_chop(std::vector<int>::const_iterator left,
               std::vector<int>::const_iterator right,
               int target) {
    if (left == right) 
        return std::vector<int>::const_iterator();
    auto middle = left + std::distance(left, right) / 2;
    if (*middle < target)
        return recursive_chop(middle + 1, right, target);
    if (*middle > target)
        return recursive_chop(left, middle, target);
    return middle;
}

ssize_t tuesday(const std::vector<int>& array, int target) {
    auto it = recursive_chop(array.cbegin(), array.cend(), target);
    if (it == std::vector<int>::const_iterator()) {
        return -1;
    }
    return std::distance(array.cbegin(), it);
}