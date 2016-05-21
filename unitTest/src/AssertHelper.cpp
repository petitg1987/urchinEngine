#include <cppunit/extensions/HelperMacros.h>

#include "AssertHelper.h"
using namespace urchin;

AssertHelper::AssertHelper()
{

}

AssertHelper::~AssertHelper()
{

}

void AssertHelper::assertTrue(bool b)
{
	CPPUNIT_ASSERT(b);
}

void AssertHelper::assertFloatEquals(float f1, float f2, float epsilon)
{
	CPPUNIT_ASSERT_MESSAGE("Assert fail. Value 1: " + std::to_string(f1) + ", value 2: " + std::to_string(f2),
			(f1 - epsilon) < f2 && (f1 + epsilon) > f2);
}

void AssertHelper::assertPoint2FloatEquals(const urchin::Point2<float> &p1, const urchin::Point2<float> &p2, float epsilon)
{
	for(unsigned int i=0; i<2; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + std::to_string(p1[i]) + ", value 2: " + std::to_string(p2[i]),
				(p1[i] - epsilon) < p2[i] && (p1[i] + epsilon) > p2[i]);
	}
}

void AssertHelper::assertPoint3FloatEquals(const urchin::Point3<float> &p1, const urchin::Point3<float> &p2, float epsilon)
{
	for(unsigned int i=0; i<3; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + std::to_string(p1[i]) + ", value 2: " + std::to_string(p2[i]),
				(p1[i] - epsilon) < p2[i] && (p1[i] + epsilon) > p2[i]);
	}
}

void AssertHelper::assertVector3FloatEquals(const urchin::Vector3<float> &v1, const urchin::Vector3<float> &v2, float epsilon)
{
	for(unsigned int i=0; i<3; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + std::to_string(v1[i]) + ", value 2: " + std::to_string(v2[i]),
				(v1[i] - epsilon) < v2[i] && (v1[i] + epsilon) > v2[i]);
	}
}

void AssertHelper::assertQuaternionFloatEquals(const urchin::Quaternion<float> &q1, const urchin::Quaternion<float> &q2, float epsilon)
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
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + std::to_string(qTest1[i]) + ", value 2: " + std::to_string(q2[i]),
				(qTest1[i] - epsilon) < q2[i] && (qTest1[i] + epsilon) > q2[i]);
	}
}

