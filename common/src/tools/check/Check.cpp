#include "Check.h"
#include "tools/ConfigService.h"

namespace urchin {

    Check::Check() :
            isAdditionalCheckEnable(ConfigService::instance()->getBoolValue("checks.additionalChecksEnable")) {

    }

    bool Check::additionalChecksEnable() {
        return isAdditionalCheckEnable;
    }

}
