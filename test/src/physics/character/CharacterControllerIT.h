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
        void ccdFallingCharacterAfterFrameDrop();
        void ccdMovingCharacter();

        void crouchAndStandUp();
        void crouchUnderCeiling();
        void crouchImmediatelyUnderCeiling();

    private:
        void constructHeightfieldFloor(urchin::PhysicsWorld&) const;
        void constructBoxFloor(urchin::PhysicsWorld&) const;
        void constructWall(urchin::PhysicsWorld&) const;
        void constructCeiling(urchin::PhysicsWorld&, float) const;
        std::unique_ptr<urchin::CharacterController> constructCharacter(urchin::PhysicsWorld&, const urchin::Point3<float>&) const;
        void simulate(urchin::PhysicsWorld&, urchin::CharacterController&, std::size_t) const;
        std::vector<std::shared_ptr<urchin::RigidBody>> constructCubes(urchin::PhysicsWorld&, float) const;
};
