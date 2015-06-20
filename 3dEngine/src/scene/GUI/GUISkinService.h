#ifndef ENGINE_GUISKINSERVICE_H
#define ENGINE_GUISKINSERVICE_H

#include "UrchinCommon.h"

#include "resources/image/Image.h"
#include "tools/xml/XmlParser.h"

namespace urchin
{

	struct WidgetOutline
	{
		int topWidth;
		int bottomWidth;
		int leftWidth;
		int rightWidth;
	};

	class GUISkinService : public Singleton<GUISkinService>
	{
		public:
			friend class Singleton<GUISkinService>;

			void setSkin(const std::string &);
		
			Image *createTexWidget(int, int, std::shared_ptr<XmlChunk>, WidgetOutline *widgetOutline=nullptr) const;
			XmlParser *getXmlSkin() const;

		private:
			GUISkinService();
			virtual ~GUISkinService();

			XmlParser *xmlSkin;
	};
}

#endif
