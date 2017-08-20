#ifndef ENGINE_ASSERTHELPER_H
#define ENGINE_ASSERTHELPER_H

#include "UrchinCommon.h"
using namespace urchin;

class AssertHelper
{
	public:
		static void assertTrue(bool);

		static void assertInt(int, int);
		static void assertUnsignedInt(unsigned int, unsigned int);
		static void assertFloatEquals(float, float, float epsilon = 0.001f);

		static void assertPoint2FloatEquals(const urchin::Point2<float> &, const urchin::Point2<float> &, double epsilon = 0.001);
		static void assertPoint3FloatEquals(const urchin::Point3<float> &, const urchin::Point3<float> &, double epsilon = 0.001);
		static void assertPoint2IntEquals(const urchin::Point2<int> &, const urchin::Point2<int> &);
		static void assertPoint2LongLongEquals(const urchin::Point2<long long> &, const urchin::Point2<long long> &);

		static void assertVector3FloatEquals(const urchin::Vector3<float> &, const urchin::Vector3<float> &, double epsilon = 0.001);

		static void assertQuaternionFloatEquals(const urchin::Quaternion<float> &, const urchin::Quaternion<float> &, double epsilon = 0.001);

	private:
		AssertHelper() = default;
		~AssertHelper() = default;

		static std::string floatToString(float);
};

#endif
