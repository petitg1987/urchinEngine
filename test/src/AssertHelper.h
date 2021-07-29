#pragma once

#include <UrchinCommon.h>
using namespace urchin;

class AssertHelper {
    public:
        static void assertTrue(bool, const std::string &msg = "");

        static void assertObjectEquals(void*, void*);

        static void assertStringEquals(const std::string&, const std::string&);
        static void assertIntEquals(int, int);
        static void assert3IntsEquals(const int*, std::array<int, 3>);
        static void assertUnsignedIntEquals(unsigned int, unsigned int);
        static void assertUnsignedIntEquals(std::size_t, std::size_t);
        static void assert3UnsignedIntsEquals(const unsigned int*, std::array<unsigned int, 3>);
        static void assert3SizesEquals(const std::size_t*, std::array<std::size_t, 3>);
        static void assertFloatEquals(float, float, float epsilon = 0.001f);

        static void assertPoint2FloatEquals(const Point2<float>&, const Point2<float>&, float epsilon = 0.001f);
        static void assertPoint2IntEquals(const Point2<int>&, const Point2<int>&);
        static void assertPoint2LongLongEquals(const Point2<long long>&, const Point2<long long>&);
        static void assertPoints2FloatEquals(const std::vector<Point2<float>>&, const std::vector<Point2<float>>&, float epsilon = 0.001f);
        static void assertPoints2LongLongEquals(const std::vector<Point2<long long>>&, const std::vector<Point2<long long>>&);

        static void assertPoint3FloatEquals(const Point3<float>&, const Point3<float>&, float epsilon = 0.001f);
        static void assertVector3FloatEquals(const Vector3<float>&, const Vector3<float>&, float epsilon = 0.001f);

        static void assertQuaternionFloatEquals(const Quaternion<float>&, const Quaternion<float>&, float epsilon = 0.001f);

        static void assertPolygonFloatEquals(std::vector<Point2<float>> , const std::vector<Point2<float>>&, float epsilon = 0.001f);
        static void assertPolygonLongLongEquals(std::vector<Point2<long long>> , const std::vector<Point2<long long>>&);

    private:
        AssertHelper() = default;
        ~AssertHelper() = default;

        static std::string floatToString(float);
};
