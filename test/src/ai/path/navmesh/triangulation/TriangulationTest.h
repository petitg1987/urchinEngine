#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinAIEngine.h>

class TriangulationTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void triangleTriangulation();
        void cubeTriangulation();
        void twoNearPoints();
        void threeAlignedPoints();
        void alternationPoints();

        void cavityTriangulation1();
        void cavityTriangulation2();

        void holeInTriangle();

    private:
        void assertUniqueLink(const urchin::NavTriangle&, std::size_t, const urchin::NavTriangle&) const;
        void assertLink(const urchin::NavLink&, std::size_t, const urchin::NavTriangle&) const;
};
