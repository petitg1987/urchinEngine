#ifndef ENGINE_NAVMESHGENERATORTEST_H
#define ENGINE_NAVMESHGENERATORTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

#include "UrchinAIEngine.h"

class NavMeshGeneratorTest : public CppUnit::TestFixture
{
    public:
        static CppUnit::Test *suite();

        void holeOnWalkableFace();
        void holeEdgeOnWalkableFace();

    private:
        urchin::NavMeshConfig buildNavMeshConfig();
};

#endif
