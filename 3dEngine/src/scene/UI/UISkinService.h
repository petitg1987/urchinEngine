#ifndef URCHINENGINE_UISKINSERVICE_H
#define URCHINENGINE_UISKINSERVICE_H

#include <map>
#include "UrchinCommon.h"

#include "resources/image/Image.h"

namespace urchin {

    struct WidgetOutline {
        unsigned int topWidth;
        unsigned int bottomWidth;
        unsigned int leftWidth;
        unsigned int rightWidth;
    };

    class UISkinService : public Singleton<UISkinService> {
        public:
            friend class Singleton<UISkinService>;

            void setSkin(const std::string&);

            std::shared_ptr<Texture> createWidgetTexture(unsigned int, unsigned int, const std::shared_ptr<XmlChunk>&, WidgetOutline* widgetOutline = nullptr) const;

            XmlParser* getXmlSkin() const;

        private:
            UISkinService();
            ~UISkinService() override;

            XmlParser* xmlSkin;
    };
}

#endif
