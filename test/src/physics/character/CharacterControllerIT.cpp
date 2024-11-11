#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <physics/character/CharacterControllerIT.h>
#include <AssertHelper.h>
using namespace urchin;

void CharacterControllerIT::fallingCharacterOnObjects() {
    auto physicsWorld = std::make_unique<PhysicsWorld>();
    constructGround(*physicsWorld);
    float cubeHeight = 0.5f;
    auto cubes = constructCubes(*physicsWorld, cubeHeight);
    auto characterShape = std::make_unique<CollisionCapsuleShape>(0.25f, 1.5f, CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y);
    float characterHeight = characterShape->getRadius() * 2.0f + characterShape->getCylinderHeight();
    auto character = std::make_unique<PhysicsCharacter>("character", 80.0f, std::move(characterShape), PhysicsTransform(Point3(2.4f, 5.0f, 2.4f), Quaternion<float>()));
    auto characterController = CharacterController(std::move(character), CharacterControllerConfig(), *physicsWorld);

    auto physicsEngineThread = std::jthread([&physicsWorld] {
        for (std::size_t i = 0; i < 300; ++i) {
            physicsWorld->getCollisionWorld().process(1.0f / 60.0f, Vector3(0.0f, -9.81f, 0.0f));
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    auto mainThread = std::jthread([&characterController] {
        for (std::size_t i = 0; i < 300; ++i) {
            characterController.update(1.0f / 60.0f);
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    physicsEngineThread.join();
    mainThread.join();

    for (const auto& cube : cubes) {
        float posY = cube->getTransform().getPosition().Y;
        float minPosY = (cubeHeight / 2.0f) - 0.01f;
        float maxPosY = cubeHeight + (cubeHeight / 2.0f) + 0.01f;
        AssertHelper::assertTrue(posY > minPosY && posY < maxPosY,"Cube " + cube->getId() + " must be on the ground or above another cube. Position Y: " + std::to_string(posY));
        AssertHelper::assertTrue(!cube->isActive(), "Cube " + cube->getId() + " must become inactive.");
    }
    float posY = characterController.getPhysicsCharacter().getTransform().getPosition().Y;
    float minPosY = (characterHeight / 2.0f) - 0.01f;
    float maxPosY = cubeHeight + cubeHeight + (characterHeight / 2.0f) + 0.01f;
    AssertHelper::assertTrue(posY > minPosY && posY < maxPosY,"Character must be on the ground or above one or two cubes. Position Y: " + std::to_string(posY));
}

void CharacterControllerIT::characterMovingOnRemovedObjects() {
    auto physicsWorld = std::make_unique<PhysicsWorld>();
    constructGround(*physicsWorld);

    std::unique_ptr<CollisionShape3D>  cubeShape = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto cubeBody = std::make_shared<RigidBody>("cube", PhysicsTransform(Point3(0.0f, 0.5f, 0.0f), Quaternion<float>()), std::move(cubeShape));
    physicsWorld->getBodyContainer().addBody(cubeBody);

    auto characterShape = std::make_unique<CollisionCapsuleShape>(0.25f, 1.5f, CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y);
    float characterHeight = characterShape->getRadius() * 2.0f + characterShape->getCylinderHeight();
    auto character = std::make_unique<PhysicsCharacter>("character", 80.0f, std::move(characterShape), PhysicsTransform(Point3(0.0f, characterHeight / 2.0f, 0.8f), Quaternion<float>()));
    CharacterControllerConfig characterControllerConfig;
    characterControllerConfig.setWalkSpeed(5.0f);
    auto characterController = CharacterController(std::move(character), characterControllerConfig, *physicsWorld);
    characterController.walk(Vector3(0.0f, 0.0f, -1.0f));

    auto physicsEngineThread = std::jthread([&physicsWorld] {
        for (std::size_t i = 0; i < 300; ++i) {
            physicsWorld->getCollisionWorld().process(1.0f / 60.0f, Vector3(0.0f, -9.81f, 0.0f));
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    auto mainThread = std::jthread([&] {
        for (std::size_t i = 0; i < 300; ++i) {
            if (i == 50) {
                physicsWorld->removeBody(*cubeBody);
                cubeBody = nullptr;
                //cubeBody could still exist because of asynchronous removed
            }
            characterController.update(1.0f / 60.0f);
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    physicsEngineThread.join();
    mainThread.join();

    AssertHelper::assertFloatEquals(characterController.getPhysicsCharacter().getTransform().getPosition().X, 0.0f, 0.1f);
    AssertHelper::assertFloatEquals(characterController.getPhysicsCharacter().getTransform().getPosition().Y, 1.0f, 0.1f);
    AssertHelper::assertFloatEquals(characterController.getPhysicsCharacter().getTransform().getPosition().Z, -20.0f, 2.0f);
}

void CharacterControllerIT::ccdFallingCharacter() {
    auto physicsWorld = std::make_unique<PhysicsWorld>();
    constructGround(*physicsWorld);
    auto characterShape = std::make_unique<CollisionCapsuleShape>(0.1f, 0.5f, CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y); //use small character to favor CCD
    float characterHeight = characterShape->getRadius() * 2.0f + characterShape->getCylinderHeight();
    auto character = std::make_unique<PhysicsCharacter>("character", 80.0f, std::move(characterShape), PhysicsTransform(Point3(0.0f, 50.0f, 0.0f), Quaternion<float>()));
    auto characterController = CharacterController(std::move(character), CharacterControllerConfig(), *physicsWorld);

    auto physicsEngineThread = std::jthread([&physicsWorld] {
        for (std::size_t i = 0; i < 250; ++i) {
            physicsWorld->getCollisionWorld().process(1.0f / 60.0f, Vector3(0.0f, -9.81f, 0.0f));
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    auto mainThread = std::jthread([&characterController] {
        for (std::size_t i = 0; i < 250; ++i) {
            characterController.update(1.0f / 35.0f); //simulate slow update to favor CCD
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    physicsEngineThread.join();
    mainThread.join();

    AssertHelper::assertFloatEquals(characterController.getPhysicsCharacter().getTransform().getPosition().Y, characterHeight / 2.0f, 0.01f);
}

void CharacterControllerIT::ccdMovingCharacter() {
    auto physicsWorld = std::make_unique<PhysicsWorld>();
    constructGround(*physicsWorld);
    constructWall(*physicsWorld);
    auto characterShape = std::make_unique<CollisionCapsuleShape>(0.1f, 0.5f, CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y); //use small character to favor CCD
    float characterHeight = characterShape->getRadius() * 2.0f + characterShape->getCylinderHeight();
    auto character = std::make_unique<PhysicsCharacter>("character", 80.0f, std::move(characterShape), PhysicsTransform(Point3(0.0f, 0.35f, 0.0f), Quaternion<float>()));
    CharacterControllerConfig characterControllerConfig;
    characterControllerConfig.setWalkSpeed(15.0f);
    auto characterController = CharacterController(std::move(character), characterControllerConfig, *physicsWorld);
    characterController.walk(Vector3(0.0f, 0.0f, -1.0f));

    auto physicsEngineThread = std::jthread([&physicsWorld] {
        for (std::size_t i = 0; i < 100; ++i) {
            physicsWorld->getCollisionWorld().process(1.0f / 60.0f, Vector3(0.0f, -9.81f, 0.0f));
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    auto mainThread = std::jthread([&characterController] {
        for (std::size_t i = 0; i < 100; ++i) {
            characterController.update(1.0f / 60.0f);
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    physicsEngineThread.join();
    mainThread.join();

    AssertHelper::assertFloatEquals(characterController.getPhysicsCharacter().getTransform().getPosition().Y, characterHeight / 2.0f, 0.01f);
    AssertHelper::assertFloatEquals(characterController.getPhysicsCharacter().getTransform().getPosition().Z, -9.75f, 0.15f);
}

void CharacterControllerIT::constructGround(PhysicsWorld& physicsWorld) const {
    std::vector groundPoints = {
            Point3(-100.0f, 0.0f, -100.0f), Point3(100.0f, 0.0f, -100.0f),
            Point3(-100.0f, 0.0f, 100.0f), Point3(100.0f, 0.0f, 100.0f)
    };
    auto groundShape = std::make_unique<CollisionHeightfieldShape>(groundPoints, 2, 2);
    auto groundBody = std::make_unique<RigidBody>("ground", PhysicsTransform(Point3(0.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(groundShape));
    physicsWorld.getBodyContainer().addBody(std::move(groundBody));
}

void CharacterControllerIT::constructWall(PhysicsWorld& physicsWorld) const {
    auto wallShape = std::make_unique<CollisionBoxShape>(Vector3(100.0f, 100.0f, 0.15f));
    auto wallBody = std::make_unique<RigidBody>("wall", PhysicsTransform(Point3(0.0f, 0.0f, -10.0f), Quaternion<float>()), std::move(wallShape));
    physicsWorld.getBodyContainer().addBody(std::move(wallBody));
}

std::vector<std::shared_ptr<RigidBody>> CharacterControllerIT::constructCubes(PhysicsWorld& physicsWorld, float cubeLength) const {
    std::vector<std::shared_ptr<RigidBody>> cubes;
    for (unsigned int x = 0; x < 5; x++) {
        float xValue = (float)x * 1.1f; //min: 0, max: 4.8
        for (unsigned int z = 0; z < 5; z++) {
            float zValue = (float)z * 1.1f; //min: 0, max: 4.8
            std::unique_ptr<CollisionShape3D>  cubeShape = std::make_unique<CollisionBoxShape>(Vector3(cubeLength / 2.0f, cubeLength / 2.0f, cubeLength / 2.0f));
            std::string bodyName = "cube_" + std::to_string(x) + "_" + std::to_string(z);
            auto cubeBody = std::make_shared<RigidBody>(bodyName, PhysicsTransform(Point3(xValue, 10.0f, zValue), Quaternion<float>()), std::move(cubeShape));
            cubeBody->setMass(10.0f); //non-static

            physicsWorld.getBodyContainer().addBody(cubeBody);
            cubes.emplace_back(cubeBody);
        }
    }
    return cubes;
}

CppUnit::Test* CharacterControllerIT::suite() {
    auto* suite = new CppUnit::TestSuite("CharacterControllerIT");

    suite->addTest(new CppUnit::TestCaller("fallingCharacterOnObjects", &CharacterControllerIT::fallingCharacterOnObjects));
    suite->addTest(new CppUnit::TestCaller("characterMovingOnRemovedObjects", &CharacterControllerIT::characterMovingOnRemovedObjects));

    suite->addTest(new CppUnit::TestCaller("ccdFallingCharacter", &CharacterControllerIT::ccdFallingCharacter));
    suite->addTest(new CppUnit::TestCaller("ccdMovingCharacter", &CharacterControllerIT::ccdMovingCharacter));

    return suite;
}