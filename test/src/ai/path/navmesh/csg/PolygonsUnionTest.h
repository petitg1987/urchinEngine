#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class PolygonsUnionTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void onePolygonUnion();

        void twoPolygonsNoUnion();
        void polygonInsideAnother();
        void polygonInsideAnotherTouchInOnePoint();

        void twoPolygonsWithHoleInside();

        void twoPolygonsUnion();
        void twoPolygonsUnionXAligned();
        void twoPolygonsUnionYAligned();
        void twoPolygonsUnionYAlmostAligned();
        void twoPolygonsUnionAlmostSameEdge1();
        void twoPolygonsUnionAlmostSameEdge2();
        void twoPolygonsUnionAlmostSameEdge3();
        void twoPolygonsUnionAlmostSameEdge4();
        void twoPolygonsUnionAlmostSameEdge5();
        void twoPolygonsUnionAlmostSameEdge6();
        void twoPolygonsTouchInOnePoint1();
        void twoPolygonsTouchInOnePoint2();
        void twoPolygonsNestedTouchInOnePoint();

        void twoIdenticalTriangles();
        void twoIdenticalSquares();
        void twoAlmostIdenticalSquares1();
        void twoAlmostIdenticalSquares2();
        void twoAlmostIdenticalSquares3();
        void twoAlmostIdenticalTrapeze();

        void threePolygonsUnion();
        void twoPolygonsUnionAndSeparatePolygon();

        void roomPolygonsUnion();
};
