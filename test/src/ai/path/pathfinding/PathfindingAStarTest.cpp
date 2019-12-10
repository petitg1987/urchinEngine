#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "PathfindingAStarTest.h"
#include "AssertHelper.h"

void PathfindingAStarTest::straightPath()
{
    //TODO impl
}

void PathfindingAStarTest::pathWithJump()
{
    //TODO impl
}

void PathfindingAStarTest::pathWithoutJump()
{
    //TODO impl
}

CppUnit::Test *PathfindingAStarTest::suite()
{
    auto *suite = new CppUnit::TestSuite("PathfindingAStarTest");

    suite->addTest(new CppUnit::TestCaller<PathfindingAStarTest>("straightPath", &PathfindingAStarTest::straightPath));

    suite->addTest(new CppUnit::TestCaller<PathfindingAStarTest>("pathWithJump", &PathfindingAStarTest::pathWithJump));
    suite->addTest(new CppUnit::TestCaller<PathfindingAStarTest>("pathWithoutJump", &PathfindingAStarTest::pathWithoutJump));

    return suite;
}
