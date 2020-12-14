#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinPhysicsEngine.h"

#include "physics/GhostBodyMT.h"
#include "AssertHelper.h"
using namespace urchin;

void GhostBodyMT::test() {
    auto physicsWorld = std::make_unique<PhysicsWorld>();
    //ground: //TODO replace by CollisionHeightfieldShape
    std::shared_ptr<CollisionBoxShape> groundShape = std::make_shared<CollisionBoxShape>(Vector3<float>(1000.0f, 0.5f, 1000.0f));
    auto* groundBody = new RigidBody("ground", Transform<float>(Point3<float>(0.0f, -0.5f, 0.0f), Quaternion<float>(), 1.0f), groundShape);
    physicsWorld->getBodyManager()->addBody(groundBody);
    //25 cubes:
    float cubeHeight = 0.5f;
    std::vector<RigidBody*> cubes;
    for(unsigned int x = 0; x < 5; x++) {
        float xValue = (float)x * 1.1f; //min: 0, max: 4.8
        for(unsigned int z = 0; z < 5; z++) {
            float zValue = (float)z * 1.1f; //min: 0, max: 4.8
            std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(cubeHeight / 2.0f, cubeHeight / 2.0f, cubeHeight / 2.0f));
            std::string bodyName = "cube_" + std::to_string(x) + "_" + std::to_string(z);
            auto* cubeBody = new RigidBody(bodyName, Transform<float>(Point3<float>(xValue, 10.0f, zValue), Quaternion<float>(), 1.0f), cubeShape);
            cubeBody->setMass(10.0f); //non-static
            physicsWorld->getBodyManager()->addBody(cubeBody);
            cubes.emplace_back(cubeBody);
        }
    }
    //character:
    std::shared_ptr<CollisionCapsuleShape> characterShape = std::make_shared<CollisionCapsuleShape>(0.5f, 1.5f, CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y);
    float characterHeight = characterShape->getRadius() * 2.0f + characterShape->getCylinderHeight();
    auto character = std::make_shared<PhysicsCharacter>("character", 80.0f, characterShape, PhysicsTransform(Point3<float>(2.4f, 5.0f, 2.4f), Quaternion<float>()));
    auto characterController = PhysicsCharacterController(character, physicsWorld.get());

    //TODO check with std::cout in CollisionCapsuleShape::toAABBox (no correct because ground is not concave ?)
    std::thread physicsEngineThread = std::thread([&physicsWorld]() {
        for(std::size_t i = 0; i < 500; ++i) {
            physicsWorld->getCollisionWorld()->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
    std::thread mainThread = std::thread([&characterController]() {
        for(std::size_t i = 0; i < 500; ++i) {
            characterController.update(1.0f / 60.0f);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
    physicsEngineThread.join();
    mainThread.join();

    for(const auto& cube : cubes) {
        float posY = cube->getTransform().getPosition().Y;
        float minPosY = (cubeHeight / 2.0f) - 0.01f;
        float maxPosY = cubeHeight + (cubeHeight / 2.0f) + 0.01f; //cube can be above another cube
        AssertHelper::assertTrue(posY > minPosY && posY < maxPosY,"Cube must be on the ground or above another cube. Position Y: " + std::to_string(posY));
        AssertHelper::assertTrue(!cube->isActive(), "Cube " + cube->getId() + " must become inactive.");
    }
    float posY = character->getTransform().getPosition().Y;
    float minPosY = (characterHeight / 2.0f) - 0.01f;
    float maxPosY = cubeHeight + cubeHeight + (characterHeight / 2.0f) + 0.01f;
    AssertHelper::assertTrue(posY > minPosY && posY < maxPosY,"Character must be on the ground or above one or two cubes. Position Y: " + std::to_string(posY));
}

CppUnit::Test* GhostBodyMT::suite() {
    auto* suite = new CppUnit::TestSuite("GhostBodyMT");

    suite->addTest(new CppUnit::TestCaller<GhostBodyMT>("test", &GhostBodyMT::test));

    return suite;
}