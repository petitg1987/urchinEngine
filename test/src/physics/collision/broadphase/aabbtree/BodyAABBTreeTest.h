#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class BodyAABBTreeTest : public CppUnit::TestFixture {
     public:
         static CppUnit::Test* suite();

         void twoBodiesPairedAndRemove();
         void twoBodiesNotPaired();

         void oneGhostBodyAndRemoveIt();
         void oneGhostBodyAndRemoveOther();
         void twoGhostBodies();

         void threeBodiesPairedAndRemove();

    private:
        void oneGhostBodyAndRemove(bool);
};
