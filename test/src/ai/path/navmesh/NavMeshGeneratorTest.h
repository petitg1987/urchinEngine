#ifndef URCHINENGINE_NAVMESHGENERATORTEST_H
#define URCHINENGINE_NAVMESHGENERATORTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <memory>

#include "UrchinAIEngine.h"

class NavMeshGeneratorTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test *suite();

        void holeOnWalkableFace();
        void holeOnWalkableFaceEdge();
        void holeOverlapOnWalkableFace();
        void holeAndCrossingHoleOnWalkableFace();

        void moveHoleOnWalkableFace();
        void removeHoleFromWalkableFace();

        void linksRecreatedAfterMove();

    private:
        unsigned int countPolygonLinks(const std::shared_ptr<urchin::NavPolygon> &, const std::shared_ptr<urchin::NavPolygon> &);
        std::shared_ptr<urchin::NavMeshAgent> buildNavMeshAgent();
};

#endif
