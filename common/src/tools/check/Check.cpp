#include "Check.h"
#include "tools/ConfigService.h"

namespace urchin
{

    Check::Check() :
            isAdditionalCheckEnable(ConfigService::instance()->getBoolValue("checks.additionalChecksEnable")) //TODO change value for release
    {

    }

    bool Check::additionalChecksEnable()
    {
        return isAdditionalCheckEnable;
    }

}
