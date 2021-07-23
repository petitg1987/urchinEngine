#pragma once

#include <UrchinCommon.h>

#include <graphic/render/target/ScreenRender.h>

namespace urchin {

    class ScreenshotService  : public Singleton<ScreenshotService> {
        public:
            friend class Singleton<ScreenshotService>;

            void takeScreenshot(const ScreenRender&, unsigned int, unsigned int) const;

        private:
            ScreenshotService() = default;

            void cmdPipelineBarrier(VkImage, VkCommandBuffer, VkAccessFlags, VkAccessFlags, VkImageLayout, VkImageLayout) const;
    };

}
