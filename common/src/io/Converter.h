#ifndef ENGINE_CONVERTER_H
#define ENGINE_CONVERTER_H

#include "math/algebra/point/Point2.h"
#include "math/algebra/point/Point3.h"
#include "math/algebra/point/Point4.h"
#include "math/algebra/vector/Vector2.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/vector/Vector4.h"

#include <string>
#include <vector>

namespace urchin
{

	class Converter
	{
		#define FLOAT_DELIMITOR ' '

		public:
			static int toInt(const std::string &);
			static unsigned int toUnsignedInt(const std::string &);
			static float toFloat(const std::string &);
			static double toDouble(const std::string &);
			static char toChar(const std::string &);

			static Point2<float> toPoint2(const std::string &);
			static Point3<float> toPoint3(const std::string &);
			static Point4<float> toPoint4(const std::string &);
			static Vector2<float> toVector2(const std::string &);
			static Vector3<float> toVector3(const std::string &);
			static Vector4<float> toVector4(const std::string &);

		private:
			Converter();
			~Converter();

			static std::vector<float> floatSplit(const std::string &, unsigned int);
	};

}

#endif
