#include "VectorCalculator.h"
#include <limits>
#include <climits>
#include <iostream>

/**
 * @brief Вычисление произведения элементов вектора
 * @param vector Вектор значений для вычисления
 * @return Произведение элементов вектора
 * @note При переполнении возвращается UINT64_MAX
 */
uint64_t VectorCalculator::computeProduct(const std::vector<uint64_t>& vector) {
    if (vector.empty()) {
        return 0;
    }

    uint64_t product = 1;

    for (uint64_t value : vector) {
        if (value == 0) {
            return 0;
        }

        // Проверка на переполнение перед умножением
        if (product > UINT64_MAX / value) {
            return UINT64_MAX; // 2^64 - 1
        }

        product *= value;
    }

    return product;
}
