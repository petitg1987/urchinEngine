#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinAIEngine.h>

class PathfindingAStarTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void straightPath();

        void joinPolygonsPath();

        void jumpWithSmallConstraint();
        void jumpWithBigConstraint();

    private:
        std::vector<urchin::PathPoint> pathWithJump(urchin::NavLinkConstraint*);
};

