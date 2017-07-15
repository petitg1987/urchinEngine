#ifndef ENGINE_POLYGONSSUBTRACTIONTEST_H
#define ENGINE_POLYGONSSUBTRACTIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class PolygonsSubtractionTest : public CppUnit::TestFixture
{
    public:
        static CppUnit::Test *suite();

        void polygonsSubtractionMiddlePoint();
        void polygonsSubtractionSameEdge();
};

#endif
