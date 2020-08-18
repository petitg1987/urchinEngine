#ifndef URCHINENGINE_GUISKINSERVICE_H
#define URCHINENGINE_GUISKINSERVICE_H

#include "UrchinCommon.h"

#include "resources/image/Image.h"
#include "tools/xml/XmlParser.h"

namespace urchin
{

    struct WidgetOutline
    {
        unsigned int topWidth;
        unsigned int bottomWidth;
        unsigned int leftWidth;
        unsigned int rightWidth;
    };

    class GUISkinService : public Singleton<GUISkinService>
    {
        public:
            friend class Singleton<GUISkinService>;

            void setSkin(const std::string &);

            std::shared_ptr<Image> createTexWidget(unsigned int, unsigned int, const std::shared_ptr<XmlChunk> &, WidgetOutline *widgetOutline=nullptr) const;
            XmlParser *getXmlSkin() const;

        private:
            GUISkinService();
            ~GUISkinService() override;

            XmlParser *xmlSkin;
    };
}

#endif
