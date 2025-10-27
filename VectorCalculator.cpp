#include "VectorCalculator.h"
#include <limits>
#include <climits>
#include <iostream>

uint64_t VectorCalculator::computeProduct(const std::vector<uint64_t>& vector) {
    if (vector.empty()) {
        return 0;
    }

    uint64_t product = 1;

    for (uint64_t value : vector) {
        if (value == 0) {
            return 0;
        }

        // Check for overflow for uint64_t
        if (product > UINT64_MAX / value) {
            return UINT64_MAX;
        }

        product *= value;
    }

    return product;
}
