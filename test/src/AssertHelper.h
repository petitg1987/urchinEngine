#pragma once

#include <UrchinCommon.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iomanip>
#include <algorithm>

class AssertHelper {
    public:
        inline static void assertTrue(bool, const std::string& = "");

        inline static void assertObjectEquals(void*, void*);

        inline static void assertStringEquals(const std::string&, const std::string&);
        inline static void assertIntEquals(int, int);
        inline static void assertUnsignedIntEquals(unsigned int, unsigned int);
        inline static void assertUnsignedIntEquals(std::size_t, std::size_t);
        inline static void assert3SizesEquals(const std::size_t*, std::array<std::size_t, 3>);
        inline static void assertFloatEquals(float, float, float epsilon = 0.001f);

        inline static void assertPoint2FloatEquals(const urchin::Point2<float>&, const urchin::Point2<float>&, float epsilon = 0.001f);
        inline static void assertPoint2IntEquals(const urchin::Point2<int>&, const urchin::Point2<int>&);
        inline static void assertPoint2LongLongEquals(const urchin::Point2<long long>&, const urchin::Point2<long long>&);
        inline static void assertPoints2FloatEquals(const std::vector<urchin::Point2<float>>&, const std::vector<urchin::Point2<float>>&, float epsilon = 0.001f);
        inline static void assertPoints2LongLongEquals(const std::vector<urchin::Point2<long long>>&, const std::vector<urchin::Point2<long long>>&);

        inline static void assertPoint3FloatEquals(const urchin::Point3<float>&, const urchin::Point3<float>&, float epsilon = 0.001f);
        inline static void assertPoint3IntEquals(const urchin::Point3<int>&, const urchin::Point3<int>&);
        inline static void assertVector3FloatEquals(const urchin::Vector3<float>&, const urchin::Vector3<float>&, float epsilon = 0.001f);

        inline static void assertQuaternionFloatEquals(const urchin::Quaternion<float>&, const urchin::Quaternion<float>&, float epsilon = 0.001f);

        inline static void assertPolygonFloatEquals(std::vector<urchin::Point2<float>>, const std::vector<urchin::Point2<float>>&, float epsilon = 0.001f);
        inline static void assertPolygonLongLongEquals(std::vector<urchin::Point2<long long>>, const std::vector<urchin::Point2<long long>>&);

    private:
        AssertHelper() = default;
        ~AssertHelper() = default;

        inline static std::string floatToString(float);
};

#include "AssertHelper.inl"
