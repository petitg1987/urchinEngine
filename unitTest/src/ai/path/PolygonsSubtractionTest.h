#ifndef ENGINE_POLYGONSSUBTRACTIONTEST_H
#define ENGINE_POLYGONSSUBTRACTIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class PolygonsSubtractionTest : public CppUnit::TestFixture
{
    public:
        static CppUnit::Test *suite();

        void polygonsSubtractionSubtrahendInside();
        void polygonsSubtractionSubtrahendInsideContactPoint();
        void polygonsSubtractionSubtrahendInsideContactEdge();
        void polygonsSubtractionSubtrahendOutside();
        void polygonsSubtractionMinuendInside();

        void polygonsSubtractionMiddlePoint();

        void polygonsSubtractionSameEdge1();
        void polygonsSubtractionSameEdge2();
        void polygonsSubtractionAlmostSamePoint();

        void polygonsSubtractionTwoSameEdge1();
        void polygonsSubtractionTwoSameEdge2();
        void polygonsSubtractionTwoSameEdge3();
        void polygonsSubtractionTwoSameEdge4();

        void polygonsSubtractionCorner();
        void polygonsSubtractionTwoIntersectionsOnEdge();

        void polygonsSubtractionMaxIteration();
};

#endif
