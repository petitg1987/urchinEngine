#ifndef URCHINENGINE_VECTORTEST_H
#define URCHINENGINE_VECTORTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

class VectorTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void triviallyCopyable();

};

#endif
