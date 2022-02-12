#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinPhysicsEngine.h>

class CollisionWorldIT : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void fallOnGround();
        void ccdPushOnGround();
        void ccdBounceOnGroundAndRoof();
        void fallForever();

        void changePositionOnInactiveBody();
        void changeMomentumOnInactiveBody();

        void changeMass();

        void rayTestWithRemovedBody();

    private:
        std::unique_ptr<urchin::BodyContainer> buildWorld(const urchin::Point3<float>&) const;
};
