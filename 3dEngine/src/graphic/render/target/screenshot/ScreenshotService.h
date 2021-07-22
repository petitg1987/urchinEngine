#pragma once

#include <UrchinCommon.h>

#include <graphic/render/target/ScreenRender.h>

namespace urchin {

    class ScreenshotService  : public Singleton<ScreenshotService> {
        public:
            friend class Singleton<ScreenshotService>;

            void takeScreenshot(const ScreenRender&) const;

        private:
            ScreenshotService() = default;
    };

}
