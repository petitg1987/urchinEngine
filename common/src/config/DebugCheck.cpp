#include "DebugCheck.h"
#include <config/ConfigService.h>

namespace urchin {

    DebugCheck::DebugCheck() :
            isAdditionalCheckEnable(ConfigService::instance()->getBoolValue("checks.additionalChecksEnable")) {

    }

    bool DebugCheck::additionalChecksEnable() const {
        return isAdditionalCheckEnable;
    }

}
