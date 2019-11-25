#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"

#include "EdgeJumpDetectionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void EdgeJumpDetectionTest::testParallelEdges()
{
    LineSegment3D<float> edge1(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));
    LineSegment3D<float> edge2(Point3<float>(0.0f, 1.0f, 0.0f), Point3<float>(5.0f, 1.0f, 0.0f));

    EdgeJumpDetection edgeJumpDetection;
    edgeJumpDetection.detectJump(edge1, edge2, 1.5f);

    //expected: Point3<float>(0.0f, 1.0f, 0.0f) & Point3<float>(5.0f, 1.0f, 0.0f)
}

CppUnit::Test *EdgeJumpDetectionTest::suite()
{
    auto *suite = new CppUnit::TestSuite("EdgeJumpDetectionTest");

    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testParallelEdges", &EdgeJumpDetectionTest::testParallelEdges));

    return suite;
}
