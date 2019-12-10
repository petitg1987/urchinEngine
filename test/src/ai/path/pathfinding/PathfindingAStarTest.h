#ifndef URCHINENGINE_PATHFINDINGASTARTEST_H
#define URCHINENGINE_PATHFINDINGASTARTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class PathfindingAStarTest : public CppUnit::TestFixture
{
    public:
        static CppUnit::Test *suite();

        void straightPath();

        void pathWithJump();
        void pathWithoutJump();
};


#endif
