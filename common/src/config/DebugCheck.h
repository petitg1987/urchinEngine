#pragma once

#include <pattern/singleton/Singleton.h>

namespace urchin {

    class DebugCheck : public Singleton<DebugCheck> {
        public:
            friend class Singleton<DebugCheck>;

            bool additionalChecksEnable() const;

        private:
            DebugCheck();
            ~DebugCheck() override = default;

            bool isAdditionalCheckEnable;
    };

}
