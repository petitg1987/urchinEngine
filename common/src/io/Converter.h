#ifndef ENGINE_CONVERTER_H
#define ENGINE_CONVERTER_H

#include <string>

namespace urchin
{

	class Converter
	{
		public:
			static int toInt(std::string);
			static unsigned int toUnsignedInt(std::string);
			static float toFloat(std::string);
			static double toDouble(std::string);
			static char toChar(std::string);

		private:
			Converter();
			~Converter();
	};

}

#endif
