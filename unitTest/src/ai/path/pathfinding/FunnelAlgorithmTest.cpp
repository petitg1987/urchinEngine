#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "FunnelAlgorithmTest.h"
#include "AssertHelper.h"

void FunnelAlgorithmTest::straightPath()
{
    const std::vector<LineSegment3D<float>> portals = {
            LineSegment3D<float>(Point3<float>(1.0, 0.0, 1.0), Point3<float>(1.0, 0.0, 1.0)), //start point
            LineSegment3D<float>(Point3<float>(0.0, 0.0, 0.0), Point3<float>(2.0, 0.0, 0.0)),
            LineSegment3D<float>(Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0)) //end point
    };

    std::vector<Point3<float>> path = FunnelAlgorithm().determinePath(portals);

    AssertHelper::assertUnsignedInt(path.size(), 2);
    AssertHelper::assertPoint3FloatEquals(path[0], Point3<float>(1.0, 0.0, 1.0));
    AssertHelper::assertPoint3FloatEquals(path[1], Point3<float>(1.0, 0.0, -1.0));
}

CppUnit::Test *FunnelAlgorithmTest::suite()
{
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("FunnelAlgorithmTest");

    suite->addTest(new CppUnit::TestCaller<FunnelAlgorithmTest>("straightPath", &FunnelAlgorithmTest::straightPath));

    return suite;
}
