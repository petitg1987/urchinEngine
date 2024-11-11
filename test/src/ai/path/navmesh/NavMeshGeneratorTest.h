#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

#include <UrchinAIEngine.h>

class NavMeshGeneratorTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void holeOnWalkableFace();
        void holeOnWalkableFaceEdge();
        void holeOverlapOnWalkableFace();
        void holeAndCrossingHoleOnWalkableFace();

        void surroundedWalkableFace();

        void moveHoleOnWalkableFace();
        void removeHoleFromWalkableFace();

        void linksRecreatedAfterMove();

    private:
        unsigned int countPolygonLinks(const urchin::NavPolygon&, const urchin::NavPolygon&) const;
        urchin::NavMeshAgent buildNavMeshAgent() const;
};
