#ifndef URCHINENGINE_EDGEJUMPDETECTIONTEST_H
#define URCHINENGINE_EDGEJUMPDETECTIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

#include "UrchinAIEngine.h"

class EdgeJumpDetectionTest: public CppUnit::TestFixture
{
    public:
        static CppUnit::Test *suite();

        void testNearParallelEdges();
        void testFarParallelEdges();

        void testIdenticalEdges();
        void testIdenticalEdgesWrongDirection();
        void testCollinearEdges1();
        void testCollinearEdges2();
        void testCollinearEdgesNoJump();
        void testCollinearEdgesWrongDirection();

        void testWrongStartJumpEdgeDirection();
        void testWrongEndJumpEdgeDirection();
        void testWrongJumpEdgesDirection();

        void testCrossEdges();

};

#endif
