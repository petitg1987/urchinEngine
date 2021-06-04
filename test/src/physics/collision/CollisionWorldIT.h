#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinPhysicsEngine.h>

class CollisionWorldIT : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void fallOnPlane();
        void fallForever();

        void changePositionOnInactiveBody();
        void changeMomentumOnInactiveBody();

        void changeMass();

    private:
        std::unique_ptr<urchin::BodyManager> buildWorld(const urchin::Point3<float>&) const;
};
