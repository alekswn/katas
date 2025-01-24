#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include "main.h"

void run_tests(ssize_t (*function)(const std::vector<int>&, int)) {
    std::vector<int> empty;
    std::vector<int> one = {1};
    std::vector<int> three = {1, 3, 5};
    std::vector<int> four = {1, 3, 5, 7};

    assert(function(empty, 3) == -1);
    assert(function(one, 3) == -1);
    assert(function(one, 1) == 0);
    assert(function(three, 1) == 0);
    assert(function(three, 3) == 1);
    assert(function(three, 5) == 2);
    assert(function(three, 0) == -1);
    assert(function(three, 2) == -1);
    assert(function(three, 4) == -1);
    assert(function(three, 6) == -1);
    assert(function(four, 1) == 0);
    assert(function(four, 3) == 1);
    assert(function(four, 5) == 2);
    assert(function(four, 7) == 3);
    assert(function(four, 0) == -1);
    assert(function(four, 2) == -1);
    assert(function(four, 4) == -1);
    assert(function(four, 6) == -1);
    assert(function(four, 8) == -1);

    std::cout << "All tests passed" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <option>" << std::endl;
        return 1;
    }

    std::string option = argv[1];

    if (option == "monday") {
        run_tests(&monday);
    } else if (option == "tuesday") {
        run_tests(&tuesday);
    } else if (option == "wednesday") {
        run_tests(&wednesday);
    } else if (option == "thursday") {
        run_tests(&thursday);
    } else {
        std::cerr << "Invalid option" << std::endl;
        return 1;
    }

    return 0;
}