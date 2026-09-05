#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

#include <UrchinCommon.h>

class MeshSimplificationServiceTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void mergeDuplicateVertices();
        void mergeDuplicateVerticesWithCollapsedTriangle();

        void downsampleVertices();

    private:
        std::array<urchin::Point3<float>, 3> extractTrianglePoints(std::size_t, const urchin::MeshData&) const;
};
