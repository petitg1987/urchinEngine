#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <memory>
#include <UrchinPhysicsEngine.h>

class CharacterControllerIT : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void fallingCharacterOnObjects();
        void ccdFallingCharacter();
        void ccdMovingCharacter();

    private:
        void constructGround(const urchin::PhysicsWorld&);
        void constructWall(const urchin::PhysicsWorld&);
        std::vector<std::shared_ptr<urchin::RigidBody>> constructCubes(const urchin::PhysicsWorld&, float);
};
