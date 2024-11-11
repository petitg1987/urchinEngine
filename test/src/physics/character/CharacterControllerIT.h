#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <memory>
#include <UrchinPhysicsEngine.h>

class CharacterControllerIT final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void fallingCharacterOnObjects();
        void characterMovingOnRemovedObjects();

        void ccdFallingCharacter();
        void ccdMovingCharacter();

    private:
        void constructGround(urchin::PhysicsWorld&) const;
        void constructWall(urchin::PhysicsWorld&) const;
        std::vector<std::shared_ptr<urchin::RigidBody>> constructCubes(urchin::PhysicsWorld&, float) const;
};
