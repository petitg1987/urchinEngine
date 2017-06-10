#include <stdexcept>

#include "NumericalCheck.h"
#include "tools/logger/Logger.h"

namespace urchin
{

    NumericalCheck::NumericalCheck() :
            Singleton<NumericalCheck>()
    {

    }

    NumericalCheck::~NumericalCheck()
    {

    }

    void NumericalCheck::perform() const
    {
        unsigned int intSize = static_cast<unsigned int>(sizeof(int));
        if(intSize < 4)
        {
            throw std::runtime_error("Wrong number of byte for int: " + std::to_string(intSize));
        }

        unsigned int longLongSize = static_cast<unsigned int>(sizeof(long long));
        if(longLongSize <= 4)
        {
            throw std::runtime_error("Wrong number of byte for long: " + std::to_string(longLongSize));
        }

        unsigned int floatSize = static_cast<unsigned int>(sizeof(float));
        if(floatSize < 4)
        {
            throw std::runtime_error("Wrong number of byte for float: " + std::to_string(floatSize));
        }

        unsigned int doubleSize = static_cast<unsigned int>(sizeof(double));
        if(doubleSize < 4)
        {
            throw std::runtime_error("Wrong number of byte for double: " + std::to_string(doubleSize));
        }else if(doubleSize == 4ui)
        {
            Logger::logger().log(Logger::WARNING, "Wrong number of byte for double: " + std::to_string(doubleSize));
        }
    }

}
