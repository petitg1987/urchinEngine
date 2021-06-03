#include <cppunit/extensions/HelperMacros.h>
#include <iomanip>
#include <algorithm>

#include <AssertHelper.h>
using namespace urchin;

void AssertHelper::assertTrue(bool b, const std::string& msg) {
    if (msg.empty()) {
        CPPUNIT_ASSERT(b);
    } else {
        CPPUNIT_ASSERT_MESSAGE(msg, b);
    }
}

void AssertHelper::assertString(const std::string& value, const std::string& expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + value + ", expected: " + expected, value == expected);
}

void AssertHelper::assertInt(int value, int expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value) + ", expected: " + std::to_string(expected), value == expected);
}

void AssertHelper::assert3Ints(const int* value, const int*&& expected) {
    for (std::size_t i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value[i]) + ", expected: " + std::to_string(expected[i]), value[i] == expected[i]);
    }
    delete[] expected;
}

void AssertHelper::assertUnsignedInt(unsigned int value, unsigned int expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value) + ", expected: " + std::to_string(expected), value == expected);
}

void AssertHelper::assertUnsignedInt(std::size_t value, std::size_t expected) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value) + ", expected: " + std::to_string(expected), value == expected);
}

void AssertHelper::assert3UnsignedInts(const unsigned int* value, unsigned int*&& expected) {
    for (std::size_t i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value[i]) + ", expected: " + std::to_string(expected[i]), value[i] == expected[i]);
    }
    delete[] expected;
}

void AssertHelper::assert3Sizes(const std::size_t* value, std::size_t*&& expected) {
    for (std::size_t i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + std::to_string(value[i]) + ", expected: " + std::to_string(expected[i]), value[i] == expected[i]);
    }
    delete[] expected;
}

void AssertHelper::assertFloatEquals(float value, float expected, double epsilon) {
    CPPUNIT_ASSERT_MESSAGE("Assert fail. Value: " + floatToString(value) + ", expected: " + floatToString(expected),
            (value - epsilon) < expected && (value + epsilon) > expected);
}

void AssertHelper::assertPoint2FloatEquals(const Point2<float>& value, const Point2<float>& expected, double epsilon) {
    for (std::size_t i = 0; i < 2; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + floatToString(value[i]) + ", expected: " + floatToString(expected[i]),
                (value[i] - epsilon) < expected[i] && (value[i] + epsilon) > expected[i]);
    }
}

void AssertHelper::assertPoint2IntEquals(const Point2<int>& value, const Point2<int>& expected) {
    for (std::size_t i = 0; i < 2; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + std::to_string(value[i]) + ", expected: " + std::to_string(expected[i]), value[i] == expected[i]);
    }
}

void AssertHelper::assertPoint2LongLongEquals(const Point2<long long>& value, const Point2<long long>& expected) {
    for (std::size_t i = 0; i < 2; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + std::to_string(value[i]) + ", expected: " + std::to_string(expected[i]), value[i] == expected[i]);
    }
}

void AssertHelper::assertPoints2FloatEquals(const std::vector<Point2<float>>& points, const std::vector<Point2<float>>& expectedPoints, double epsilon) {
    AssertHelper::assertUnsignedInt(points.size(), expectedPoints.size());

    for (std::size_t i = 0; i < points.size(); ++i) {
        assertPoint2FloatEquals(points[i], expectedPoints[i], epsilon);
    }
}

void AssertHelper::assertPoints2LongLongEquals(const std::vector<Point2<long long>>& points, const std::vector<Point2<long long>>& expectedPoints) {
    AssertHelper::assertUnsignedInt(points.size(), expectedPoints.size());

    for (std::size_t i = 0; i < points.size(); ++i) {
        assertPoint2LongLongEquals(points[i], expectedPoints[i]);
    }
}

void AssertHelper::assertPoint3FloatEquals(const Point3<float>& value, const Point3<float>& expected, double epsilon) {
    for (std::size_t i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + floatToString(value[i]) + ", expected: " + floatToString(expected[i]),
                               (value[i] - epsilon) < expected[i] && (value[i] + epsilon) > expected[i]);
    }
}

void AssertHelper::assertVector3FloatEquals(const Vector3<float>& value, const Vector3<float>& expected, double epsilon) {
    for (std::size_t i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + floatToString(value[i]) + ", expected: " + floatToString(expected[i]),
                (value[i] - epsilon) < expected[i] && (value[i] + epsilon) > expected[i]);
    }
}

void AssertHelper::assertQuaternionFloatEquals(const Quaternion<float>& value, const Quaternion<float>& expected, double epsilon) {
    Quaternion<float> qTest1(value);
    if ((value.W < 0.0 && expected.W > 0.0) || (value.W > 0.0 && expected.W < 0.0)) {
        qTest1.X = -qTest1.X;
        qTest1.Y = -qTest1.Y;
        qTest1.Z = -qTest1.Z;
        qTest1.W = -qTest1.W;
    }

    for (unsigned int i = 0; i < 4; ++i) {
        CPPUNIT_ASSERT_MESSAGE("Assert fail on axis: " + std::to_string(i) + ". Value: " + floatToString(qTest1[i]) + ", expected: " + floatToString(expected[i]),
                (qTest1[i] - epsilon) < expected[i] && (qTest1[i] + epsilon) > expected[i]);
    }
}

void AssertHelper::assertPolygonFloatEquals(std::vector<Point2<float>> orientedPoints, const std::vector<Point2<float>>& orientedExpectedPoints, double epsilon) {
    AssertHelper::assertUnsignedInt(orientedPoints.size(), orientedExpectedPoints.size());

    for (std::size_t i = 0; i < orientedPoints.size(); ++i) {
        try {
            assertPoints2FloatEquals(orientedPoints, orientedExpectedPoints, epsilon);
        } catch (std::exception& e) {
            std::rotate(orientedPoints.begin(), orientedPoints.begin() + 1, orientedPoints.end());
            continue;
        }

        return;
    }

    assertTrue(false);
}

void AssertHelper::assertPolygonLongLongEquals(std::vector<Point2<long long>> orientedPoints, const std::vector<Point2<long long>>& orientedExpectedPoints) {
    AssertHelper::assertUnsignedInt(orientedPoints.size(), orientedExpectedPoints.size());

    for (std::size_t i = 0; i < orientedPoints.size(); ++i) {
        try {
            assertPoints2LongLongEquals(orientedPoints, orientedExpectedPoints);
        } catch (std::exception& e) {
            std::rotate(orientedPoints.begin(), orientedPoints.begin() + 1, orientedPoints.end());
            continue;
        }

        return;
    }

    assertTrue(false);
}

std::string AssertHelper::floatToString(float f) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(10) << f;
    return stream.str();
}

