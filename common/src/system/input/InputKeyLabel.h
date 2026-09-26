#pragma once

#include <string>
#include <map>

#include "system/input/InputKey.h"

namespace urchin {

    class InputKeyLabel {
        public:
            static const std::map<InputKey, std::string>& getLabels();

        private:
            static std::map<InputKey, std::string> buildLabels();
    };

}
