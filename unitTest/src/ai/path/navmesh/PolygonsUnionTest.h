#ifndef URCHINENGINE_POLYGONSUNIONTEST_H
#define URCHINENGINE_POLYGONSUNIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class PolygonsUnionTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void onePolygonUnion();

		void twoPolygonsNoUnion();
		void polygonInsideAnother();
		void polygonInsideAnotherTouchInOnePoint();

		void twoPolygonsUnion();
		void twoPolygonsUnionXAligned();
		void twoPolygonsUnionYAligned();
		void twoPolygonsUnionYAlmostAligned();
		void twoPolygonsUnionAlmostSameEdge1();
        void twoPolygonsUnionAlmostSameEdge2();
		void twoPolygonsUnionAlmostSameEdge3();
		void twoPolygonsUnionAlmostSameEdge4();
		void twoPolygonsUnionAlmostSameEdge5();
		void twoPolygonsSameEndPoint();
		void twoPolygonsIntersectionIsEndPoint();
		void twoPolygonsIntersectionIsAlmostEndPoint();
		void twoPolygonsIntersectionIsAlmostEdgeA1();
		void twoPolygonsIntersectionIsAlmostEdgeA2();
		void twoPolygonsTouchInOnePoint();
		void twoPolygonsNestedTouchInOnePoint();
		void twoPolygonsIntersectionLastEdge();

		void twoIdenticalTriangles();
		void twoIdenticalSquares();
		void twoAlmostIdenticalSquares1();
		void twoAlmostIdenticalSquares2();
		void twoAlmostIdenticalSquares3();
		void twoAlmostIdenticalTrapeze();

		void threePolygonsUnion();
		void twoPolygonsUnionAndSeparatePolygon();

		void test(); //TODO review name
};

#endif
