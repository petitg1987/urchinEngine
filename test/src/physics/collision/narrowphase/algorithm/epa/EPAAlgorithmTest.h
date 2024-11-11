#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class EPAAlgorithmTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        //AABBox
        void identicalAABBox();
        void overlapAABBox();

        //OBBox
        void identicalOBBox();
        void overlapOnEdgeOBBox();
        void overlapOnCornerOBBox();

        //Convex hull
        void identicalBox();
        void cornerInsideBox();
        void faceInsideTrapeze();
        void cornerInsideHexagon();

        //Sphere
        void identicalSphere();
        void overlapSphere();

        //Different shape type
        void overlapOBBoxAABBox();
        void overlapSphereAndBox();
        void overlapCapsuleAndTriangle();
        void overlapTriangleAndCapsule();

    private:
        urchin::EPAResult<float> executeEPA(const urchin::CollisionConvexObject3D&, const urchin::CollisionConvexObject3D&) const;
};
