#include <cppunit/extensions/HelperMacros.h>
#include <iomanip>
#include <algorithm>

#include "AssertHelper.h"
using namespace urchin;

void AssertHelper::assertTrue(bool b, const std::string &msg)
{
    if(msg=="")
    {
        CPPUNIT_ASSERT(b);
    }else
    {
        CPPUNIT_ASSERT_MESSAGE(msg, b);
    }
}

void AssertHelper::assertInt(int i1, int i2)
{
	CPPUNIT_ASSERT_MESSAGE("Assert fail. Value 1: " + std::to_string(i1) + ", value 2: " + std::to_string(i2), i1==i2);
}

void AssertHelper::assert3Ints(const int *i1, const int *&&i2)
{
	for(unsigned int i=0; i<3; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail. Value 1: " + std::to_string(i1[i]) + ", value 2: " + std::to_string(i2[i]), i1[i] == i2[i]);
	}
	delete [] i2;
}

void AssertHelper::assertUnsignedInt(unsigned int ui1, unsigned int ui2)
{
	CPPUNIT_ASSERT_MESSAGE("Assert fail. Value 1: " + std::to_string(ui1) + ", value 2: " + std::to_string(ui2), ui1==ui2);
}

void AssertHelper::assert3UnsignedInts(const unsigned int *ui1, unsigned int *&&ui2)
{
	for(unsigned int i=0; i<3; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail. Value 1: " + std::to_string(ui1[i]) + ", value 2: " + std::to_string(ui2[i]), ui1[i] == ui2[i]);
	}
	delete [] ui2;
}

void AssertHelper::assertFloatEquals(float f1, float f2, double epsilon)
{
	CPPUNIT_ASSERT_MESSAGE("Assert fail. Value 1: " + floatToString(f1) + ", value 2: " + floatToString(f2),
			(f1 - epsilon) < f2 && (f1 + epsilon) > f2);
}

void AssertHelper::assertPoint2FloatEquals(const Point2<float> &p1, const Point2<float> &p2, double epsilon)
{
	for(unsigned int i=0; i<2; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + floatToString(p1[i]) + ", value 2: " + floatToString(p2[i]),
				(p1[i] - epsilon) < p2[i] && (p1[i] + epsilon) > p2[i]);
	}
}

void AssertHelper::assertPoint2IntEquals(const Point2<int> &p1, const Point2<int> &p2)
{
	for(unsigned int i=0; i<2; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + std::to_string(p1[i]) + ", value 2: " + std::to_string(p2[i]), p1[i]==p2[i]);
	}
}

void AssertHelper::assertPoint2LongLongEquals(const Point2<long long> &p1, const Point2<long long> &p2)
{
	for(unsigned int i=0; i<2; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + std::to_string(p1[i]) + ", value 2: " + std::to_string(p2[i]), p1[i]==p2[i]);
	}
}

void AssertHelper::assertPoints2FloatEquals(const std::vector<Point2<float>> &points, const std::vector<Point2<float>> &expectedPoints, double epsilon)
{
    AssertHelper::assertUnsignedInt(points.size(), expectedPoints.size());

    for(unsigned int i=0; i<points.size(); ++i)
    {
        assertPoint2FloatEquals(points[i], expectedPoints[i], epsilon);
    }
}

void AssertHelper::assertPoints2LongLongEquals(const std::vector<Point2<long long>> &points, const std::vector<Point2<long long>> &expectedPoints)
{
	AssertHelper::assertUnsignedInt(points.size(), expectedPoints.size());

	for(unsigned int i=0; i<points.size(); ++i)
	{
		assertPoint2LongLongEquals(points[i], expectedPoints[i]);
	}
}

void AssertHelper::assertPoint3FloatEquals(const Point3<float> &p1, const Point3<float> &p2, double epsilon)
{
    for(unsigned int i=0; i<3; ++i)
    {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + floatToString(p1[i]) + ", value 2: " + floatToString(p2[i]),
                               (p1[i] - epsilon) < p2[i] && (p1[i] + epsilon) > p2[i]);
    }
}

void AssertHelper::assertVector3FloatEquals(const Vector3<float> &v1, const Vector3<float> &v2, double epsilon)
{
	for(unsigned int i=0; i<3; ++i)
	{
		CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value 1: " + floatToString(v1[i]) + ", value 2: " + floatToString(v2[i]),
				(v1[i] - epsilon) < v2[i] && (v1[i] + epsilon) > v2[i]);
	}
}

void AssertHelper::assertQuaternionFloatEquals(const Quaternion<float> &q1, const Quaternion<float> &q2, double epsilon)
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

void AssertHelper::assertPolygonFloatEquals(std::vector<Point2<float>> orientedPoints, const std::vector<Point2<float>> &orientedExpectedPoints, double epsilon)
{
    AssertHelper::assertUnsignedInt(orientedPoints.size(), orientedExpectedPoints.size());

    for(unsigned int i=0; i<orientedPoints.size(); ++i)
    {
        try
        {
            assertPoints2FloatEquals(orientedPoints, orientedExpectedPoints, epsilon);
        }catch(std::exception &e)
        {
            std::rotate(orientedPoints.begin(), orientedPoints.begin() + 1, orientedPoints.end());
            continue;
        }

        return;
    }

    assertTrue(false);
}

void AssertHelper::assertPolygonLongLongEquals(std::vector<Point2<long long>> orientedPoints, const std::vector<Point2<long long>> &orientedExpectedPoints)
{
    AssertHelper::assertUnsignedInt(orientedPoints.size(), orientedExpectedPoints.size());

    for(unsigned int i=0; i<orientedPoints.size(); ++i)
    {
        try
        {
            assertPoints2LongLongEquals(orientedPoints, orientedExpectedPoints);
        }catch(std::exception &e)
        {
            std::rotate(orientedPoints.begin(), orientedPoints.begin() + 1, orientedPoints.end());
            continue;
        }

        return;
    }

    assertTrue(false);
}

std::string AssertHelper::floatToString(float f)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(10) << f;
	return stream.str();
}

