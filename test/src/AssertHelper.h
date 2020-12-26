#ifndef URCHINENGINE_ASSERTHELPER_H
#define URCHINENGINE_ASSERTHELPER_H

#include "UrchinCommon.h"
using namespace urchin;

class AssertHelper : public Singleton<AssertHelper> {
    public:
        friend class Singleton<AssertHelper>;

        void assertTrue(bool, const std::string &msg = "");

        void assertString(const std::string&, const std::string&);
        void assertInt(int, int);
        void assert3Ints(const int*, const int *&&);
        void assertUnsignedInt(unsigned int, unsigned int);
        void assertUnsignedInt(std::size_t, std::size_t);
        void assert3UnsignedInts(const unsigned int*, unsigned int *&&);
        void assert3Sizes(const std::size_t*, std::size_t *&&);
        void assertFloatEquals(float, float, double epsilon = 0.001);

        void assertPoint2FloatEquals(const Point2<float>&, const Point2<float>&, double epsilon = 0.001);
        void assertPoint2IntEquals(const Point2<int>&, const Point2<int>&);
        void assertPoint2LongLongEquals(const Point2<long long>&, const Point2<long long>&);
        void assertPoints2FloatEquals(const std::vector<Point2<float>>&, const std::vector<Point2<float>>&, double epsilon = 0.001);
        void assertPoints2LongLongEquals(const std::vector<Point2<long long>>&, const std::vector<Point2<long long>>&);

        void assertPoint3FloatEquals(const Point3<float>&, const Point3<float>&, double epsilon = 0.001);
        void assertVector3FloatEquals(const Vector3<float>&, const Vector3<float>&, double epsilon = 0.001);

        void assertQuaternionFloatEquals(const Quaternion<float>&, const Quaternion<float>&, double epsilon = 0.001);

        void assertPolygonFloatEquals(std::vector<Point2<float>> , const std::vector<Point2<float>>&, double epsilon = 0.001);
        void assertPolygonLongLongEquals(std::vector<Point2<long long>> , const std::vector<Point2<long long>>&);

    private:
        AssertHelper() = default;
        ~AssertHelper() override = default;

        std::string floatToString(float);
};

#endif
