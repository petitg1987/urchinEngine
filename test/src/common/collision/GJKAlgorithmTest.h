#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class GJKAlgorithmTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        //AABBox
        void identicalAABBox();
        void separateAABBox();
        void overlapAABBox();

        //OBBox
        void identicalOBBox();
        void separateEdgeOBBox();
        void separateCornerOBBox();
        void overlapOnEdgeOBBox();

        //Convex hull
        void identicalBox();
        void separateBox();
        void cornerInsideBox();
        void faceInsideTrapeze();
        void separateHexagon();
        void cornerInsideHexagon();

        //Sphere
        void identicalSphere();
        void separateSphere();
        void overlapSphere();

        //Different shape type
        void separateOBBoxAABBox();
        void overlapOBBoxAABBox();
        void separateSphereAndAABBox();
        void overlapSphereAndAABBox();
};
