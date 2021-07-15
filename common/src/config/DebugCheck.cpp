#include <config/DebugCheck.h>
#include <config/ConfigService.h>

namespace urchin {

    bool DebugCheck::additionalChecksEnable() {
        static bool isAdditionalCheckEnable = ConfigService::instance().getBoolValue("checks.additionalChecksEnable");
        return isAdditionalCheckEnable;
    }

}
