#ifndef URCHINENGINE_EDGELINKDETECTIONTEST_H
#define URCHINENGINE_EDGELINKDETECTIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

#include "UrchinAIEngine.h"

class EdgeLinkDetectionTest: public CppUnit::TestFixture
{
    public:
        static CppUnit::Test *suite();

        void testNearParallelEdges();
        void testFarParallelEdges();

        void testIdenticalEdges();
        void testIdenticalEdgesWrongDirection();
        void testCollinearEdges1();
        void testCollinearEdges2();
        void testCollinearEdges3();
        void testCollinearEdgesNoLink();
        void testCollinearEdgesTouchingInOnePoint();
        void testCollinearEdgesWrongDirection();

        void testWrongStartJumpEdgeDirection();
        void testWrongEndJumpEdgeDirection();
        void testWrongJumpEdgesDirection();

        void testCrossEdges();

};

#endif
