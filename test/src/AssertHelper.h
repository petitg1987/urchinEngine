#pragma once

#include <UrchinCommon.h>
#include <cppunit/extensions/HelperMacros.h>

class AssertHelper {
    public:
        static void assertTrue(bool, const std::string& = "");
        static void assertFalse(bool, const std::string& = "");

        static void assertNull(const void*);
        static void assertNotNull(const void*);
        static void assertObjectEquals(const void*, const void*);

        static void assertStringEquals(const std::string&, const std::string&);
        static void assertIntEquals(int, int);
        static void assertUnsignedIntEquals(unsigned int, unsigned int);
        static void assertUnsignedIntEquals(std::size_t, std::size_t);
        static void assert3SizesEquals(const std::array<std::size_t, 3>&, const std::array<std::size_t, 3>&);
        static void assertFloatEquals(float, float, float epsilon = 0.001f);

        static void assertPoint2FloatEquals(const urchin::Point2<float>&, const urchin::Point2<float>&, float epsilon = 0.001f);
        static void assertPoint2IntEquals(const urchin::Point2<int>&, const urchin::Point2<int>&);
        static void assertPoint2LongLongEquals(const urchin::Point2<long long>&, const urchin::Point2<long long>&);
        static void assertPoints2FloatEquals(const std::vector<urchin::Point2<float>>&, const std::vector<urchin::Point2<float>>&, float epsilon = 0.001f);
        static void assertPoints2LongLongEquals(const std::vector<urchin::Point2<long long>>&, const std::vector<urchin::Point2<long long>>&);

        static void assertPoint3FloatEquals(const urchin::Point3<float>&, const urchin::Point3<float>&, float epsilon = 0.001f);
        static void assertPoint3IntEquals(const urchin::Point3<int>&, const urchin::Point3<int>&);
        static void assertVector3FloatEquals(const urchin::Vector3<float>&, const urchin::Vector3<float>&, float epsilon = 0.001f);

        static void assertQuaternionFloatEquals(const urchin::Quaternion<float>&, const urchin::Quaternion<float>&, float epsilon = 0.001f);

        static void assertPolygonFloatEquals(std::vector<urchin::Point2<float>>, const std::vector<urchin::Point2<float>>&, float epsilon = 0.001f);
        static void assertPolygonLongLongEquals(std::vector<urchin::Point2<long long>>, const std::vector<urchin::Point2<long long>>&);

    private:
        AssertHelper() = default;
        ~AssertHelper() = default;

        static std::string floatToString(float);
};

#include "AssertHelper.inl"
