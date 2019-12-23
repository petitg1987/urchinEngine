#ifndef URCHINENGINE_BODYAABBTREETEST_H
#define URCHINENGINE_BODYAABBTREETEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class BodyAABBTreeTest : public CppUnit::TestFixture
{ //TODO add more test
     public:
         static CppUnit::Test *suite();

         void twoBodiesPairedAndRemoved();
         void twoBodiesNotPaired();

         void oneBodyWithAlternativePair();
         void twoBodiesWithAlternativePair();
};

#endif
