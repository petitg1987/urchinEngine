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
	CPPUNIT_ASSERT( (f1 - epsilon) < f2 && (f1 + epsilon) > f2);
}

void AssertHelper::assertPoint2FloatEquals(const urchin::Point2<float> &p1, const urchin::Point2<float> &p2, float epsilon)
{
	for(unsigned int i=0; i<2; ++i)
	{
		CPPUNIT_ASSERT( (p1[i] - epsilon) < p2[i] && (p1[i] + epsilon) > p2[i]);
	}
}

void AssertHelper::assertPoint3FloatEquals(const urchin::Point3<float> &p1, const urchin::Point3<float> &p2, float epsilon)
{
	for(unsigned int i=0; i<3; ++i)
	{
		CPPUNIT_ASSERT( (p1[i] - epsilon) < p2[i] && (p1[i] + epsilon) > p2[i]);
	}
}

void AssertHelper::assertVector3FloatEquals(const urchin::Vector3<float> &v1, const urchin::Vector3<float> &v2, float epsilon)
{
	for(unsigned int i=0; i<3; ++i)
	{
		CPPUNIT_ASSERT( (v1[i] - epsilon) < v2[i] && (v1[i] + epsilon) > v2[i]);
	}
}
