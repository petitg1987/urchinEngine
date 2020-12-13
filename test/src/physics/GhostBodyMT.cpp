#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinPhysicsEngine.h"

#include "physics/GhostBodyMT.h"
#include "AssertHelper.h"
using namespace urchin;

void GhostBodyMT::test() {
    auto* physicsWorld = new PhysicsWorld();
    //plane / ground:
    std::shared_ptr<CollisionBoxShape> planeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(1000.0f, 0.5f, 1000.0f));
    auto* planeBody = new RigidBody("plane", Transform<float>(Point3<float>(0.0f, -0.5f, 0.0f), Quaternion<float>(), 1.0f), planeShape);
    physicsWorld->getBodyManager()->addBody(planeBody);
    //25 cubes:
    std::vector<RigidBody*> cubes;
    for(unsigned int x = 0; x < 5; x++) {
        float xValue = (float)x * 1.1f; //min: 0, max: 4.8
        for(unsigned int z = 0; z < 5; z++) {
            float zValue = (float)z * 1.1f; //min: 0, max: 4.8
            std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
            std::string bodyName = "cube_" + std::to_string(x) + "_" + std::to_string(z);
            auto* cubeBody = new RigidBody(bodyName, Transform<float>(Point3<float>(xValue, 10.0f, zValue), Quaternion<float>(), 1.0f), cubeShape);
            cubeBody->setMass(10.0f); //non-static
            physicsWorld->getBodyManager()->addBody(cubeBody);
            cubes.emplace_back(cubeBody);
        }
    }
    //character:
    std::shared_ptr<CollisionCapsuleShape> characterShape = std::make_shared<CollisionCapsuleShape>(0.5f, 1.5f, CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y);
    auto character = std::make_shared<PhysicsCharacter>("character", 80.0f, characterShape, PhysicsTransform(Point3<float>(2.4f, 5.0f, 2.4f), Quaternion<float>()));
    auto characterController = PhysicsCharacterController(character, physicsWorld);

    //TODO parallel execution (+check with std::cout in CollisionCapsuleShape::toAABBox)
    for (std::size_t i = 0; i < 500; ++i) {
        physicsWorld->getCollisionWorld()->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
    }
    for(std::size_t i = 0; i < 500; ++i) {
        characterController.update(1.0f / 60.0f);
    }

    for(const auto& cube : cubes) {
        float cubePositionY = cube->getTransform().getPosition().Y;
        AssertHelper::assertTrue(cubePositionY > 0.4f && cubePositionY < 3.0f, "Cube " + cube->getId() + " must be on the plane or above another cube. Position Y: " + std::to_string(cubePositionY));
        AssertHelper::assertTrue(!cube->isActive(), "Cube " + cube->getId() + " must become inactive.");
    }
    std::cout<<character->getTransform().getPosition()<<std::endl; //TODO check character position

    delete physicsWorld;
}

CppUnit::Test* GhostBodyMT::suite() {
    auto* suite = new CppUnit::TestSuite("GhostBodyMT");

    suite->addTest(new CppUnit::TestCaller<GhostBodyMT>("test", &GhostBodyMT::test));

    return suite;
}