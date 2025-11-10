#include <UnitTest++/UnitTest++.h>
#include "VectorCalculator.h"
#include <vector>
#include <cstdint>
#include <climits>

SUITE(VectorCalculatorTest)
{
    TEST(EmptyVector) {
        std::vector<uint64_t> emptyVec;
        uint64_t result = VectorCalculator::computeProduct(emptyVec);
        CHECK_EQUAL(0, result);
    }
    
    TEST(SingleElement) {
        std::vector<uint64_t> vec = {42};
        uint64_t result = VectorCalculator::computeProduct(vec);
        CHECK_EQUAL(42, result);
    }
    
    TEST(MultipleElements) {
        std::vector<uint64_t> vec = {2, 3, 4};
        uint64_t result = VectorCalculator::computeProduct(vec);
        CHECK_EQUAL(24, result);
    }
    
    TEST(WithZero) {
        std::vector<uint64_t> vec = {1, 2, 0, 4};
        uint64_t result = VectorCalculator::computeProduct(vec);
        CHECK_EQUAL(0, result);
    }
    
    TEST(OverflowDetection) {
        std::vector<uint64_t> vec = {UINT64_MAX, 2};
        uint64_t result = VectorCalculator::computeProduct(vec);
        CHECK_EQUAL(UINT64_MAX, result);
    }
    
    TEST(LargeNumbers) {
        std::vector<uint64_t> vec = {1000000, 1000000};
        uint64_t result = VectorCalculator::computeProduct(vec);
        CHECK_EQUAL(1000000000000, result);
    }
}
