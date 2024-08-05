#pragma once

#include <iostream>
#include <limits>
#include <cmath>


struct Statistics {
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();
    double mean = 0.0;
    double m2 = 0.0;
    size_t count = 0;

    void update(double value) {
        count++;

        // Update minimum and maximum
        if (value < min) min = value;
        if (value > max) max = value;

        // Update mean
        double delta = value - mean;
        mean += delta / count;

        // Update variance and standard deviation
        double delta2 = value - mean;
        m2 += delta * delta2;
    }

    double variance() const {
        return m2 / (count - 1);
    }

    double standardDeviation() const {
        return sqrt(variance());
    }
};