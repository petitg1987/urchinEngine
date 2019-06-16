#include <utility>

#include "RequestException.h"

namespace urchin
{
    RequestException::RequestException(std::string message) :
        message(std::move(message))
    {

    }

    const char * RequestException::what() const noexcept
    {
        return message.c_str();
    }
}
