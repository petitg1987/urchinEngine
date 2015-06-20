#ifndef ENGINE_MAPEDITOR_LABELSTYLEHELPER_H
#define ENGINE_MAPEDITOR_LABELSTYLEHELPER_H

#include <string>
#include <QtWidgets/QLabel>

namespace urchin
{

	class LabelStyleHelper
	{
		public:
			static void applyErrorStyle(QLabel *, const std::string &);
			static void applyNormalStyle(QLabel *);
	};

}

#endif
