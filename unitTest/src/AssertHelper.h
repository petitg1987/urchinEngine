#ifndef ENGINE_ASSERTHELPER_H
#define ENGINE_ASSERTHELPER_H

#include "UrchinCommon.h"
using namespace urchin;

class AssertHelper
{
	public:
		static void assertTrue(bool);
		static void assertFloatEquals(float, float, float epsilon = 0.001f);
		static void assertPoint2FloatEquals(const urchin::Point2<float> &, const urchin::Point2<float> &, float epsilon = 0.001f);
		static void assertPoint3FloatEquals(const urchin::Point3<float> &, const urchin::Point3<float> &, float epsilon = 0.001f);
		static void assertVector3FloatEquals(const urchin::Vector3<float> &, const urchin::Vector3<float> &, float epsilon = 0.001f);
		static void assertQuaternionFloatEquals(const urchin::Quaternion<float> &, const urchin::Quaternion<float> &, float epsilon = 0.001f);

	private:
		AssertHelper();
		~AssertHelper();
};

#endif
