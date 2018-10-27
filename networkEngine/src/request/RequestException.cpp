#include "RequestException.h"

namespace urchin
{
    RequestException::RequestException(const std::string& message) :
        message(message)
    {

    }

    const char * RequestException::what() const noexcept
    {
        return message.c_str();
    }
}
