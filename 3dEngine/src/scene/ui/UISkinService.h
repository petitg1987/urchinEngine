#pragma once

#include <memory>
#include <UrchinCommon.h>

#include "graphics/api/GraphicsApi.h"
#include "scene/ui/widget/LengthType.h"

namespace urchin {

    struct WidgetOutline {
        int topWidth = 0;
        int bottomWidth = 0;
        int leftWidth = 0;
        int rightWidth = 0;
    };

    struct Length {
        float value;
        LengthType type;
    };

    class UISkinService {
        public:
            static UISkinService& instance();

            void setSkin(const std::string&);

            std::shared_ptr<Texture> createWidgetTexture(unsigned int, unsigned int, const UdaChunk*, WidgetOutline* widgetOutline = nullptr) const;
            Length loadLength(const UdaChunk*, std::string_view) const;

            const UdaParser& getSkinReader() const;

        private:
            UISkinService();

            std::unique_ptr<UdaParser> skinReader;
    };
}
