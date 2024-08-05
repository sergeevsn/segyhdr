#pragma once
#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <cmath>
#include <iomanip>
#include <limits>
#include <unordered_map>
#include "segy.h"
#include "stats.h"

// Function to display a progress bar
void progressBar(size_t current, size_t total, int barWidth = 80) {
    std::cout << "[";
    int pos = barWidth * current / total;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(current * 100.0 / total) << " %\r";
    std::cout.flush();
}

void clearProgressBar(int barWidth = 80) {
    // Output enough spaces to overwrite the progress bar
    for (int i = 0; i < barWidth + 10; ++i) {
        std::cout << " ";
    }
    // Move the cursor back to the beginning of the line
    std::cout << "\r";
    std::cout.flush();
}


std::vector<std::pair<int, int>> parseHdrParams(const std::string& input) {
    std::vector<std::pair<int, int>> result;
    std::stringstream ss(input);
    std::string item;

    while (std::getline(ss, item, ',')) {
        std::stringstream pairStream(item);
        std::string first, second;
        if (std::getline(pairStream, first, ':') && std::getline(pairStream, second, ':')) {
            result.emplace_back(std::stoi(first), std::stoi(second));
        }
    }

    return result;
}

std::vector<int> parse_number_params(const std::string& param_string) {
    std::vector<int> numbers;
    std::stringstream ss(param_string);
    std::string segment;

    while (std::getline(ss, segment, ',')) {
        size_t first_colon_pos = segment.find(':');
        size_t second_colon_pos = segment.find(':', first_colon_pos + 1);

        if (second_colon_pos != std::string::npos) {
            // Range with step
            int start = std::stoi(segment.substr(0, first_colon_pos));
            int end = std::stoi(segment.substr(first_colon_pos + 1, second_colon_pos - first_colon_pos - 1));
            int step = std::stoi(segment.substr(second_colon_pos + 1));

            for (int i = start; i <= end; i += step) {
                numbers.push_back(i);
            }
        }
        else if (first_colon_pos != std::string::npos) {
            // Range without step
            int start = std::stoi(segment.substr(0, first_colon_pos));
            int end = std::stoi(segment.substr(first_colon_pos + 1));

            for (int i = start; i <= end; ++i) {
                numbers.push_back(i);
            }
        }
        else {
            // Individual number
            numbers.push_back(std::stoi(segment));
        }
    }

    return numbers;
}