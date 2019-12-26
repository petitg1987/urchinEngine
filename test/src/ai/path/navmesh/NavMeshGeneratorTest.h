#ifndef URCHINENGINE_NAVMESHGENERATORTEST_H
#define URCHINENGINE_NAVMESHGENERATORTEST_H

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
        void holeOverlapOnWalkableFace();

        void moveHoleOnWalkableFace();
        void removeHoleFromWalkableFace();

        void linksRecreatedAfterMove();

    private:
        bool polygonsAreLinked(const std::shared_ptr<urchin::NavPolygon> &sourcePolygon, const std::shared_ptr<urchin::NavPolygon> &targetPolygon);
        std::shared_ptr<urchin::NavMeshAgent> buildNavMeshAgent();
};

#endif
