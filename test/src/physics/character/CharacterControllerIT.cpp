#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <physics/character/CharacterControllerIT.h>
#include <AssertHelper.h>
using namespace urchin;

void CharacterControllerIT::fallingCharacterOnObjects() {
    auto physicsWorld = std::make_unique<PhysicsWorld>();
    constructGround(physicsWorld);
    float cubeHeight = 0.5f;
    auto cubes = constructCubes(physicsWorld, cubeHeight);
    std::shared_ptr<CollisionCapsuleShape> characterShape = std::make_shared<CollisionCapsuleShape>(0.25f, 1.5f, CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y);
    float characterHeight = characterShape->getRadius() * 2.0f + characterShape->getCylinderHeight();
    auto character = std::make_shared<PhysicsCharacter>("character", 80.0f, characterShape, PhysicsTransform(Point3<float>(2.4f, 5.0f, 2.4f), Quaternion<float>()));
    auto characterController = CharacterController(character, CharacterControllerConfig(), physicsWorld.get());

    std::thread physicsEngineThread = std::thread([&physicsWorld]() {
        for (std::size_t i = 0; i < 500; ++i) {
            physicsWorld->getCollisionWorld()->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    std::thread mainThread = std::thread([&characterController]() {
        for (std::size_t i = 0; i < 500; ++i) {
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
    float posY = character->getTransform().getPosition().Y;
    float minPosY = (characterHeight / 2.0f) - 0.01f;
    float maxPosY = cubeHeight + cubeHeight + (characterHeight / 2.0f) + 0.01f;
    AssertHelper::assertTrue(posY > minPosY && posY < maxPosY,"Character must be on the ground or above one or two cubes. Position Y: " + std::to_string(posY));
}

void CharacterControllerIT::ccdFallingCharacter() {
    auto physicsWorld = std::make_unique<PhysicsWorld>();
    constructGround(physicsWorld);
    std::shared_ptr<CollisionCapsuleShape> characterShape = std::make_shared<CollisionCapsuleShape>(0.1f, 0.5f, CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y); //use small character to favor CCD
    float characterHeight = characterShape->getRadius() * 2.0f + characterShape->getCylinderHeight();
    auto character = std::make_shared<PhysicsCharacter>("character", 80.0f, characterShape, PhysicsTransform(Point3<float>(0.0f, 50.0f, 0.0f), Quaternion<float>()));
    auto characterController = CharacterController(character, CharacterControllerConfig(), physicsWorld.get());

    std::thread physicsEngineThread = std::thread([&physicsWorld]() {
        for (std::size_t i = 0; i < 250; ++i) {
            physicsWorld->getCollisionWorld()->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    std::thread mainThread = std::thread([&characterController]() {
        for (std::size_t i = 0; i < 250; ++i) {
            characterController.update(1.0f / 35.0f); //simulate slow update to favor CCD
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    physicsEngineThread.join();
    mainThread.join();

    AssertHelper::assertFloatEquals(character->getTransform().getPosition().Y, characterHeight / 2.0f, 0.01f);
}

void CharacterControllerIT::ccdMovingCharacter() {
    auto physicsWorld = std::make_unique<PhysicsWorld>();
    constructGround(physicsWorld);
    constructWall(physicsWorld);
    std::shared_ptr<CollisionCapsuleShape> characterShape = std::make_shared<CollisionCapsuleShape>(0.1f, 0.5f, CapsuleShape<float>::CapsuleOrientation::CAPSULE_Y); //use small character to favor CCD
    float characterHeight = characterShape->getRadius() * 2.0f + characterShape->getCylinderHeight();
    auto character = std::make_shared<PhysicsCharacter>("character", 80.0f, characterShape, PhysicsTransform(Point3<float>(0.0f, 0.35f, 0.0f), Quaternion<float>()));
    auto characterController = CharacterController(character, CharacterControllerConfig(), physicsWorld.get());
    characterController.setVelocity(Vector3<float>(0.0f, 0.0f, -15.0f));

    std::thread physicsEngineThread = std::thread([&physicsWorld]() {
        for (std::size_t i = 0; i < 100; ++i) {
            physicsWorld->getCollisionWorld()->process(1.0f / 60.0f, Vector3<float>(0.0f, -9.81f, 0.0f));
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    std::thread mainThread = std::thread([&characterController]() {
        for (std::size_t i = 0; i < 100; ++i) {
            characterController.update(1.0f / 60.0f);
            std::this_thread::sleep_for(std::chrono::microseconds(250));
        }
    });
    physicsEngineThread.join();
    mainThread.join();

    AssertHelper::assertFloatEquals(character->getTransform().getPosition().Y, characterHeight / 2.0f, 0.01f);
    AssertHelper::assertFloatEquals(character->getTransform().getPosition().Z, -9.75f, 0.1f);
}

void CharacterControllerIT::constructGround(const std::unique_ptr<PhysicsWorld>& physicsWorld) {
    std::vector<Point3<float>> groundPoints = {
            Point3<float>(-100.0f, 0.0f, -100.0f), Point3<float>(100.0f, 0.0f, -100.0f),
            Point3<float>(-100.0f, 0.0f, 100.0f), Point3<float>(100.0f, 0.0f, 100.0f)
    };
    std::shared_ptr<CollisionHeightfieldShape> groundShape = std::make_shared<CollisionHeightfieldShape>(groundPoints, 2, 2);
    auto* groundBody = new RigidBody("ground", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), groundShape);
    physicsWorld->getBodyManager()->addBody(groundBody);
}

void CharacterControllerIT::constructWall(const std::unique_ptr<PhysicsWorld>& physicsWorld) {
    std::shared_ptr<CollisionBoxShape> wallShape = std::make_shared<CollisionBoxShape>(Vector3<float>(100.0f, 100.0f, 0.15f));
    auto* wallBody = new RigidBody("wall", PhysicsTransform(Point3<float>(0.0f, 0.0f, -10.0f), Quaternion<float>()), wallShape);
    physicsWorld->getBodyManager()->addBody(wallBody);
}

std::vector<RigidBody*> CharacterControllerIT::constructCubes(const std::unique_ptr<PhysicsWorld>& physicsWorld, float cubeHeight) {
    std::vector<RigidBody*> cubes;
    for (unsigned int x = 0; x < 5; x++) {
        float xValue = (float)x * 1.1f; //min: 0, max: 4.8
        for (unsigned int z = 0; z < 5; z++) {
            float zValue = (float)z * 1.1f; //min: 0, max: 4.8
            std::shared_ptr<CollisionShape3D>  cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(cubeHeight / 2.0f, cubeHeight / 2.0f, cubeHeight / 2.0f));
            std::string bodyName = "cube_" + std::to_string(x) + "_" + std::to_string(z);
            auto* cubeBody = new RigidBody(bodyName, PhysicsTransform(Point3<float>(xValue, 10.0f, zValue), Quaternion<float>()), cubeShape);
            cubeBody->setMass(10.0f); //non-static

            physicsWorld->getBodyManager()->addBody(cubeBody);
            cubes.emplace_back(cubeBody);
        }
    }
    return cubes;
}

CppUnit::Test* CharacterControllerIT::suite() {
    auto* suite = new CppUnit::TestSuite("CharacterControllerIT");

    suite->addTest(new CppUnit::TestCaller<CharacterControllerIT>("fallingCharacterOnObjects", &CharacterControllerIT::fallingCharacterOnObjects));
    suite->addTest(new CppUnit::TestCaller<CharacterControllerIT>("ccdFallingCharacter", &CharacterControllerIT::ccdFallingCharacter));
    suite->addTest(new CppUnit::TestCaller<CharacterControllerIT>("ccdMovingCharacter", &CharacterControllerIT::ccdMovingCharacter));

    return suite;
}