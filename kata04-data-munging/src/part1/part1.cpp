#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>

int main() {
    std::ifstream file("data/weather.dat");
    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    std::string line;
    int minSpreadDay = 0;
    int minSpread = std::numeric_limits<int>::max();

    // Skip the header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int day, maxTemp, minTemp;
        if (!(iss >> day >> maxTemp >> minTemp)) {
            continue; // Skip lines that don't match the expected format
        }

        int spread = maxTemp - minTemp;
        if (spread < minSpread) {
            minSpread = spread;
            minSpreadDay = day;
        }
    }

    file.close();

    std::cout << "Day with the smallest temperature spread: " << minSpreadDay << std::endl;
    return 0;
}
