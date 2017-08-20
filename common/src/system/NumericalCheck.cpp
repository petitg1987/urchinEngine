#include <stdexcept>

#include "NumericalCheck.h"
#include "tools/logger/Logger.h"

namespace urchin
{

    void NumericalCheck::perform() const
    {
        auto intSize = static_cast<unsigned int>(sizeof(int));
        if(intSize < 4)
        {
            throw std::runtime_error("Wrong number of byte for int: " + std::to_string(intSize));
        }

        auto longLongSize = static_cast<unsigned int>(sizeof(long long));
        if(longLongSize <= 4)
        {
            throw std::runtime_error("Wrong number of byte for long long: " + std::to_string(longLongSize));
        }

        auto floatSize = static_cast<unsigned int>(sizeof(float));
        if(floatSize < 4)
        {
            throw std::runtime_error("Wrong number of byte for float: " + std::to_string(floatSize));
        }

        auto doubleSize = static_cast<unsigned int>(sizeof(double));
        if(doubleSize < 8)
        {
            throw std::runtime_error("Wrong number of byte for double: " + std::to_string(doubleSize));
        }

        if(floatSize >= doubleSize)
        { //some float algorithms require a higher precision to work correctly
            throw std::runtime_error("Wrong number of byte for float/double: " + std::to_string(floatSize) + "/" + std::to_string(doubleSize));
        }
    }

}
