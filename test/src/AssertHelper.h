#ifndef URCHINENGINE_ASSERTHELPER_H
#define URCHINENGINE_ASSERTHELPER_H

#include <UrchinCommon.h>
using namespace urchin;

class AssertHelper {
    public:
        static void assertTrue(bool, const std::string &msg = "");

        static void assertString(const std::string&, const std::string&);
        static void assertInt(int, int);
        static void assert3Ints(const int*, const int *&&);
        static void assertUnsignedInt(unsigned int, unsigned int);
        static void assertUnsignedInt(std::size_t, std::size_t);
        static void assert3UnsignedInts(const unsigned int*, unsigned int *&&);
        static void assert3Sizes(const std::size_t*, std::size_t *&&);
        static void assertFloatEquals(float, float, double epsilon = 0.001);

        static void assertPoint2FloatEquals(const Point2<float>&, const Point2<float>&, double epsilon = 0.001);
        static void assertPoint2IntEquals(const Point2<int>&, const Point2<int>&);
        static void assertPoint2LongLongEquals(const Point2<long long>&, const Point2<long long>&);
        static void assertPoints2FloatEquals(const std::vector<Point2<float>>&, const std::vector<Point2<float>>&, double epsilon = 0.001);
        static void assertPoints2LongLongEquals(const std::vector<Point2<long long>>&, const std::vector<Point2<long long>>&);

        static void assertPoint3FloatEquals(const Point3<float>&, const Point3<float>&, double epsilon = 0.001);
        static void assertVector3FloatEquals(const Vector3<float>&, const Vector3<float>&, double epsilon = 0.001);

        static void assertQuaternionFloatEquals(const Quaternion<float>&, const Quaternion<float>&, double epsilon = 0.001);

        static void assertPolygonFloatEquals(std::vector<Point2<float>> , const std::vector<Point2<float>>&, double epsilon = 0.001);
        static void assertPolygonLongLongEquals(std::vector<Point2<long long>> , const std::vector<Point2<long long>>&);

    private:
        AssertHelper() = default;
        ~AssertHelper() = default;

        static std::string floatToString(float);
};

#endif
