#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <memory>

#include <UrchinAIEngine.h>

class NavMeshGeneratorTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void holeOnWalkableFace();
        void holeOnWalkableFaceEdge();
        void holeOverlapOnWalkableFace();
        void holeAndCrossingHoleOnWalkableFace();

        void moveHoleOnWalkableFace();
        void removeHoleFromWalkableFace();

        void linksRecreatedAfterMove();

    private:
        unsigned int countPolygonLinks(const urchin::NavPolygon&, const urchin::NavPolygon&);
        std::unique_ptr<urchin::NavMeshAgent> buildNavMeshAgent();
};
