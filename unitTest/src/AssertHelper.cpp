#include <cppunit/extensions/HelperMacros.h>
#include <sstream>
#include <iomanip>

#include "AssertHelper.h"
using namespace urchin;

void AssertHelper::assertTrue(bool b)
{
	CPPUNIT_ASSERT(b);
}

void AssertHelper::assertInt(int i1, int i2)
{
	CPPUNIT_ASSERT_MESSAGE("Assert fail. Value 1: " + std::to_string(i1) + ", value 2: " + std::to_string(i2), i1==i2);
}

void AssertHelper::assertUnsignedInt(unsigned int ui1, unsigned int ui2)
{
	CPPUNIT_ASSERT_MESSAGE("Assert fail. Value 1: " + std::to_string(ui1) + ", value 2: " + std::to_string(ui2), ui1==ui2);
}

void AssertHelper::assertFloatEquals(float f1, float f2, float epsilon)
{
	CPPUNIT_ASSERT_MESSAGE("Assert fail. Value 1: " + floatToString(f1) + ", value 2: " + floatToString(f2),
			(f1 - epsilon) < f2 && (f1 + epsilon) > f2);
}

void AssertHelper::assertPoint2FloatEquals(const urchin::Point2<float> &p1, const urchin::Point2<float> &p2, double epsilon)
{
	for(unsigned int i=0; i<2; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + floatToString(p1[i]) + ", value 2: " + floatToString(p2[i]),
				(p1[i] - epsilon) < p2[i] && (p1[i] + epsilon) > p2[i]);
	}
}

void AssertHelper::assertPoint3FloatEquals(const urchin::Point3<float> &p1, const urchin::Point3<float> &p2, double epsilon)
{
	for(unsigned int i=0; i<3; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + floatToString(p1[i]) + ", value 2: " + floatToString(p2[i]),
				(p1[i] - epsilon) < p2[i] && (p1[i] + epsilon) > p2[i]);
	}
}

void AssertHelper::assertPoint2IntEquals(const urchin::Point2<int> &p1, const urchin::Point2<int> &p2)
{
	for(unsigned int i=0; i<2; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + std::to_string(p1[i]) + ", value 2: " + std::to_string(p2[i]), p1[i]==p2[i]);
	}
}

void AssertHelper::assertVector3FloatEquals(const urchin::Vector3<float> &v1, const urchin::Vector3<float> &v2, double epsilon)
{
	for(unsigned int i=0; i<3; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + floatToString(v1[i]) + ", value 2: " + floatToString(v2[i]),
				(v1[i] - epsilon) < v2[i] && (v1[i] + epsilon) > v2[i]);
	}
}

void AssertHelper::assertQuaternionFloatEquals(const urchin::Quaternion<float> &q1, const urchin::Quaternion<float> &q2, double epsilon)
{
	Quaternion<float> qTest1(q1);
	if((q1.W < 0.0 && q2.W > 0.0) || (q1.W > 0.0 && q2.W < 0.0))
	{
		qTest1.X = -qTest1.X;
		qTest1.Y = -qTest1.Y;
		qTest1.Z = -qTest1.Z;
		qTest1.W = -qTest1.W;
	}

	for(unsigned int i=0; i<4; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + floatToString(qTest1[i]) + ", value 2: " + floatToString(q2[i]),
				(qTest1[i] - epsilon) < q2[i] && (qTest1[i] + epsilon) > q2[i]);
	}
}

std::string AssertHelper::floatToString(float f)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(10) << f;
	return stream.str();
}

