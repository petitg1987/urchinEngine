#pragma once

#include <map>
#include <memory>
#include <UrchinCommon.h>

#include <resources/image/Image.h>
#include <scene/ui/widget/Length.h>

namespace urchin {

    struct WidgetOutline {
        int topWidth = 0;
        int bottomWidth = 0;
        int leftWidth = 0;
        int rightWidth = 0;
    };

    class UISkinService : public Singleton<UISkinService> {
        public:
            friend class Singleton<UISkinService>;

            void setSkin(const std::string&);

            std::shared_ptr<Texture> createWidgetTexture(unsigned int, unsigned int, const XmlChunk*, WidgetOutline* widgetOutline = nullptr) const;
            Length loadLength(const XmlChunk*, const std::string&) const;

            const std::unique_ptr<XmlParser>& getXmlSkin() const;

        private:
            UISkinService();

            std::unique_ptr<XmlParser> xmlSkin;
    };
}
