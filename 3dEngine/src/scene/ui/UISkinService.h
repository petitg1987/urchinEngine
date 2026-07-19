#pragma once

#include <memory>
#include <UrchinCommon.h>

#include "graphics/api/GraphicsApi.h"
#include "scene/ui/widget/LengthType.h"
#include "scene/ui/Outline.h"

namespace urchin {

    struct Length {
        float value;
        LengthType type;
    };

    class UISkinService {
        public:
            static UISkinService& instance();

            void setSkin(const std::string&);

            std::shared_ptr<Texture> createWidgetTexture(unsigned int, unsigned int, const UdaChunk*, Outline* widgetOutline = nullptr) const;
            Length loadLength(const UdaChunk*, std::string_view) const;

            const UdaParser& getSkinReader() const;

        private:
            UISkinService();

            std::unique_ptr<UdaParser> skinReader;
    };
}
