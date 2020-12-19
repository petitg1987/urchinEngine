#ifndef URCHINENGINE_COLLISIONWORLDIT_H
#define URCHINENGINE_COLLISIONWORLDIT_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class CollisionWorldIT : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void fallOnPlane();
        void fallForever();
};

#endif
