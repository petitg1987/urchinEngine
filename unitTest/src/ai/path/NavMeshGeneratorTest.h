#ifndef ENGINE_NAVMESHGENERATORTEST_H
#define ENGINE_NAVMESHGENERATORTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <memory>

#include "UrchinAIEngine.h"

class NavMeshGeneratorTest : public CppUnit::TestFixture
{
    public:
        static CppUnit::Test *suite();

        void holeOnWalkableFace();
        void holeEdgeOnWalkableFace();

    private:
        std::shared_ptr<urchin::NavMeshConfig> buildNavMeshConfig();
};

#endif
