#ifndef URCHINENGINE_BODYAABBTREETEST_H
#define URCHINENGINE_BODYAABBTREETEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class BodyAABBTreeTest : public CppUnit::TestFixture
{
     public:
         static CppUnit::Test *suite();

         void twoBodiesPairedAndRemove();
         void twoBodiesNotPaired();

         void oneBodyWithAlternativePairAndRemoveIt();
         void oneBodyWithAlternativePairAndRemoveOther();
         void twoBodiesWithAlternativePair();

         void threeBodiesPairedAndRemove();

    private:
        void oneBodyWithAlternativePairAndRemove(bool);
};

#endif
