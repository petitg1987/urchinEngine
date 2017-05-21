#ifndef ENGINE_LINESEGMENT2DCOLLISIONTEST_H
#define ENGINE_LINESEGMENT2DCOLLISIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class LineSegment2DCollisionTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void noIntersection();
		void parallelLines();
		void intersection();

		void intersectionOnOnePoint();
		void collinearIntersectionOnOnePoint();

		void collinearLinesNoIntersectionSameDirection();
		void collinearLinesNoIntersectionOppositeDirection();

		void collinearLinesIntersectionSameDirection();
		void collinearLinesIntersectionSameDirection2();
		void collinearLinesIntersectionOppositeDirection();
		void collinearLinesIntersectionOppositeDirection2();
		void collinearLinesFullIntersectionSameDirection();
		void collinearLinesFullIntersectionOppositeDirection();
};

#endif
