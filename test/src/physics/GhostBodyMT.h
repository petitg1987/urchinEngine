#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <memory>
#include <UrchinPhysicsEngine.h>

class GhostBodyMT : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void processGhostBody();

    private:
        void constructGround(const std::unique_ptr<urchin::PhysicsWorld>&);
        std::vector<urchin::RigidBody*> constructCubes(const std::unique_ptr<urchin::PhysicsWorld>&, float);
};
