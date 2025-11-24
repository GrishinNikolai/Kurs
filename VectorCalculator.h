#ifndef VECTORCALCULATOR_H
#define VECTORCALCULATOR_H

#include <vector>
#include <cstdint>

/**
 * @brief Класс для вычисления произведения элементов вектора
 * @author Гришин Николай
 * @version 1.0
 * @date 24.11.2025
 * @copyright ИБСТ ПГУ
 */
class VectorCalculator {
public:
    /**
     * @brief Вычисление произведения элементов вектора
     * @param vector Вектор значений для вычисления
     * @return Произведение элементов вектора
     * @note При переполнении возвращается UINT64_MAX
     */
    static uint64_t computeProduct(const std::vector<uint64_t>& vector);
};

#endif
