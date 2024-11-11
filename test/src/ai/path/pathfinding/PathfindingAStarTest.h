#pragma once

#include <memory>
#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinAIEngine.h>

class PathfindingAStarTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void straightPath();

        void joinPolygonsPath();

        void jumpWithSmallConstraint();
        void jumpWithBigConstraint();

    private:
        std::vector<urchin::PathPoint> pathWithJump(std::unique_ptr<urchin::NavLinkConstraint>);
};

