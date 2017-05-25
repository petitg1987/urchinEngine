#ifndef ENGINE_POLYGONSUNIONTEST_H
#define ENGINE_POLYGONSUNIONTEST_H

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
		void twoPolygonsSameEndPoint();
		void twoPolygonsIntersectionIsEndPoint();
		void twoPolygonsIntersectionIsAlmostEdgeA();
		void twoPolygonsTouchInOnePoint();
		void twoPolygonsNestedTouchInOnePoint();
		void twoPolygonsIntersectionLastEdge();

		void twoIdenticalTriangles();
		void twoIdenticalSquares();
		void twoAlmostIdenticalSquares();
		void twoAlmostIdenticalSquares2();

		void threePolygonsUnion();
		void twoPolygonsUnionAndSeparatePolygon();
};

#endif
