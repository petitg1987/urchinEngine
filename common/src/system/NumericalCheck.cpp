#include <stdexcept>

#include "NumericalCheck.h"

namespace urchin {

    void NumericalCheck::perform() {
        auto intSize = (unsigned int)sizeof(int);
        if (intSize < 4) {
            throw std::runtime_error("Wrong number of byte for int: " + std::to_string(intSize));
        }

        auto longLongSize = (unsigned int)sizeof(long long);
        if (longLongSize <= 4) {
            throw std::runtime_error("Wrong number of byte for long long: " + std::to_string(longLongSize));
        }

        auto floatSize = (unsigned int)sizeof(float);
        if (floatSize < 4) {
            throw std::runtime_error("Wrong number of byte for float: " + std::to_string(floatSize));
        }

        auto doubleSize = (unsigned int)sizeof(double);
        if (doubleSize < 8) {
            throw std::runtime_error("Wrong number of byte for double: " + std::to_string(doubleSize));
        }

        if (floatSize >= doubleSize) { //some float algorithms require a higher precision to work correctly
            throw std::runtime_error("Wrong number of byte for float/double: " + std::to_string(floatSize) + "/" + std::to_string(doubleSize));
        }
    }

}
