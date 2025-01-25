#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include "main.h"

void benchmark(ssize_t (*function)(const std::vector<int>&, int), const std::vector<int>& array, int target, const std::string& name) {
    double total_duration = 0.0;
    for (int i = 0; i < 100; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        function(array, target);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        total_duration += duration.count();
    }

    std::cout << name << ": " << (total_duration / 100) << " seconds (average over 100 runs)" << std::endl;
}

int main() {
    std::vector<int> array(1000000000);
    std::iota(array.begin(), array.end(), 0);  // Fill the vector with values from 0 to 999,999,999
    int target = 999999;

    benchmark(monday, array, target, "Monday");
    benchmark(tuesday, array, target, "Tuesday");
    benchmark(wednesday, array, target, "Wednesday");
    benchmark(thursday, array, target, "Thursday");
    benchmark(friday, array, target, "Friday");

    return 0;
}
