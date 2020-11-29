#ifndef URCHINENGINE_POLYGONSSUBTRACTIONTEST_H
#define URCHINENGINE_POLYGONSSUBTRACTIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class PolygonsSubtractionTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void polygonsSubtractionSubtrahendInside();
        void polygonsSubtractionSubtrahendInsideContactPoint();
        void polygonsSubtractionSubtrahendInsideContactEdge();
        void polygonsSubtractionSubtrahendOutside();
        void polygonsSubtractionMinuendInside();
        void polygonsSubtractionMinuendInsideContactEdge();

        void polygonsSubtractionMiddlePoint();

        void polygonsSubtractionSameEdge1();
        void polygonsSubtractionSameEdge2();
        void polygonsSubtractionSameEdge3();
        void polygonsSubtractionSameEdge4();

        void polygonsSubtractionTwoSameEdge1();
        void polygonsSubtractionTwoSameEdge2();
        void polygonsSubtractionTwoSameEdge3();
        void polygonsSubtractionTwoSameEdge4();
        void polygonsSubtractionTwoSameEdge5();

        void polygonsSubtractionMaxIteration();

        void polygonsSubtractionTwoIntersectionsOnEdge();
        void polygonsSubtractionCorner1();
        void polygonsSubtractionCorner2();
        void polygonsSubtractionCorner3();
        void polygonsSubtractionCorner4();
};

#endif
