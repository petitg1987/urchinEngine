#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinPhysicsEngine.h>

#include "AssertHelper.h"
#include "physics/collision/broadphase/aabbtree/BodyAABBTreeTest.h"
using namespace urchin;

void BodyAABBTreeTest::twoBodiesPairedAndRemove() {
    //add bodies test:
    auto cubeShapeA = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_shared<RigidBody>("bodyA", PhysicsTransform(Point3(0.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeA));
    auto cubeShapeB = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto bodyB = std::make_shared<RigidBody>("bodyB", PhysicsTransform(Point3(1.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeB));
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA);
    bodyAabbTree.addBody(bodyB);
    const auto& bodyANodeData = static_cast<BodyAABBNodeData&>(bodyAabbTree.getNodeData(bodyA.get()));

    AssertHelper::assertUnsignedIntEquals(bodyAabbTree.getOverlappingPairs().size(), 1);
    AssertHelper::assertStringEquals(bodyAabbTree.getOverlappingPairs()[0]->getBody1().getId(), "bodyB");
    AssertHelper::assertStringEquals(bodyAabbTree.getOverlappingPairs()[0]->getBody2().getId(), "bodyA");
    AssertHelper::assertUnsignedIntEquals(bodyANodeData.getOwnerPairContainers().size(), 0);

    //remove a body test:
    bodyAabbTree.removeBody(*bodyB);
    AssertHelper::assertUnsignedIntEquals(bodyAabbTree.getOverlappingPairs().size(), 0);
}

void BodyAABBTreeTest::twoBodiesNotPaired() {
    auto cubeShapeA = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_shared<RigidBody>("bodyA", PhysicsTransform(Point3(0.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeA));
    auto cubeShapeB = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto bodyB = std::make_shared<RigidBody>("bodyB", PhysicsTransform(Point3(10.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeB));
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA);
    bodyAabbTree.addBody(bodyB);

    AssertHelper::assertUnsignedIntEquals(bodyAabbTree.getOverlappingPairs().size(), 0);
}

void BodyAABBTreeTest::oneGhostBodyAndRemoveIt() {
    oneGhostBodyAndRemove(true);
}

void BodyAABBTreeTest::oneGhostBodyAndRemoveOther() {
    oneGhostBodyAndRemove(false);
}

void BodyAABBTreeTest::oneGhostBodyAndRemove(bool removeGhostBody) {
    //add bodies test:
    auto cubeShapeA = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_shared<RigidBody>("bodyA", PhysicsTransform(Point3(0.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeA));
    auto cubeShapeB = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto bodyB = std::make_shared<GhostBody>("bodyB", PhysicsTransform(Point3(1.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeB));
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA);
    bodyAabbTree.addBody(bodyB);
    const auto& bodyANodeData = static_cast<BodyAABBNodeData&>(bodyAabbTree.getNodeData(bodyA.get()));
    const auto& bodyBNodeData = static_cast<BodyAABBNodeData&>(bodyAabbTree.getNodeData(bodyB.get()));

    AssertHelper::assertUnsignedIntEquals(bodyAabbTree.getOverlappingPairs().size(), 0);
    std::vector<OverlappingPair> bodyBPairs;
    bodyB->getPairContainer()->retrieveCopyOverlappingPairs(bodyBPairs);
    AssertHelper::assertUnsignedIntEquals(bodyBPairs.size(), 1);
    AssertHelper::assertStringEquals(bodyBPairs[0].getBody1().getId(), "bodyB");
    AssertHelper::assertStringEquals(bodyBPairs[0].getBody2().getId(), "bodyA");
    AssertHelper::assertUnsignedIntEquals(bodyANodeData.getOwnerPairContainers().size(), 1);
    AssertHelper::assertUnsignedIntEquals(bodyBNodeData.getOwnerPairContainers().size(), 0);

    //remove a body test:
    if (removeGhostBody) {
        bodyAabbTree.removeBody(*bodyB);
        const auto& bodyANodeDataAfterRemove = static_cast<BodyAABBNodeData&>(bodyAabbTree.getNodeData(bodyA.get()));
        std::vector<OverlappingPair> updateBodyBPairs;
        bodyB->getPairContainer()->retrieveCopyOverlappingPairs(updateBodyBPairs);
        AssertHelper::assertTrue(updateBodyBPairs.empty());
        AssertHelper::assertUnsignedIntEquals(bodyANodeDataAfterRemove.getOwnerPairContainers().size(), 0);
    } else {
        bodyAabbTree.removeBody(*bodyA);
        std::vector<OverlappingPair> updateBodyBPairs;
        bodyB->getPairContainer()->retrieveCopyOverlappingPairs(updateBodyBPairs);
        AssertHelper::assertTrue(updateBodyBPairs.empty());
    }
}

void BodyAABBTreeTest::twoGhostBodies() {
    auto cubeShapeA = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_shared<GhostBody>("bodyA", PhysicsTransform(Point3(0.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeA));
    auto cubeShapeB = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto bodyB = std::make_shared<GhostBody>("bodyB", PhysicsTransform(Point3(1.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeB));
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA);
    bodyAabbTree.addBody(bodyB);

    AssertHelper::assertTrue(bodyAabbTree.getOverlappingPairs().empty());
    std::vector<OverlappingPair> bodyAPairs;
    bodyA->getPairContainer()->retrieveCopyOverlappingPairs(bodyAPairs);
    AssertHelper::assertTrue(bodyAPairs.empty()); //ghost bodies do not see each other
    std::vector<OverlappingPair> bodyBPairs;
    bodyB->getPairContainer()->retrieveCopyOverlappingPairs(bodyBPairs);
    AssertHelper::assertTrue(bodyBPairs.empty()); //ghost bodies do not see each other
}

void BodyAABBTreeTest::threeBodiesPairedAndRemove() {
    //add bodies test:
    auto cubeShapeA = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_shared<RigidBody>("bodyA", PhysicsTransform(Point3(0.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeA));
    auto cubeShapeB = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto bodyB = std::make_shared<GhostBody>("bodyB", PhysicsTransform(Point3(1.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeB));
    auto cubeShapeC = std::make_unique<CollisionBoxShape>(Vector3(0.5f, 0.5f, 0.5f));
    auto bodyC = std::make_shared<GhostBody>("bodyC", PhysicsTransform(Point3(0.0f, 1.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeC));
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA);
    bodyAabbTree.addBody(bodyB);
    bodyAabbTree.addBody(bodyC);

    std::vector<OverlappingPair> bodyBPairs;
    std::vector<OverlappingPair> bodyCPairs;
    bodyB->getPairContainer()->retrieveCopyOverlappingPairs(bodyBPairs);
    bodyC->getPairContainer()->retrieveCopyOverlappingPairs(bodyCPairs);
    AssertHelper::assertUnsignedIntEquals(bodyAabbTree.getOverlappingPairs().size(), 0);
    AssertHelper::assertUnsignedIntEquals(bodyBPairs.size(), 1);
    AssertHelper::assertObjectEquals(&bodyBPairs[0].getBody2(), bodyA.get());
    AssertHelper::assertUnsignedIntEquals(bodyCPairs.size(), 1);
    AssertHelper::assertObjectEquals(&bodyCPairs[0].getBody2(), bodyA.get());

    //remove a body test:
    std::vector<OverlappingPair> bodyPairs;
    bodyAabbTree.removeBody(*bodyB);
    bodyPairs.clear();
    bodyB->getPairContainer()->retrieveCopyOverlappingPairs(bodyPairs);
    AssertHelper::assertTrue(bodyPairs.empty());
    bodyPairs.clear();
    bodyC->getPairContainer()->retrieveCopyOverlappingPairs(bodyPairs);
    AssertHelper::assertUnsignedIntEquals(bodyPairs.size(), 1);
    bodyAabbTree.removeBody(*bodyA);
    bodyPairs.clear();
    bodyB->getPairContainer()->retrieveCopyOverlappingPairs(bodyPairs);
    AssertHelper::assertTrue(bodyPairs.empty());
    bodyPairs.clear();
    bodyC->getPairContainer()->retrieveCopyOverlappingPairs(bodyPairs);
    AssertHelper::assertTrue(bodyPairs.empty());
    bodyAabbTree.removeBody(*bodyC);
    bodyPairs.clear();
    bodyB->getPairContainer()->retrieveCopyOverlappingPairs(bodyPairs);
    AssertHelper::assertTrue(bodyPairs.empty());
    bodyPairs.clear();
    bodyC->getPairContainer()->retrieveCopyOverlappingPairs(bodyPairs);
    AssertHelper::assertTrue(bodyPairs.empty());
}

CppUnit::Test* BodyAABBTreeTest::suite() {
    auto* suite = new CppUnit::TestSuite("BodyAABBTreeTest");

    suite->addTest(new CppUnit::TestCaller("twoBodiesPairedAndRemove", &BodyAABBTreeTest::twoBodiesPairedAndRemove));
    suite->addTest(new CppUnit::TestCaller("twoBodiesNotPaired", &BodyAABBTreeTest::twoBodiesNotPaired));

    suite->addTest(new CppUnit::TestCaller("oneGhostBodyAndRemoveIt", &BodyAABBTreeTest::oneGhostBodyAndRemoveIt));
    suite->addTest(new CppUnit::TestCaller("oneGhostBodyAndRemoveOther", &BodyAABBTreeTest::oneGhostBodyAndRemoveOther));
    suite->addTest(new CppUnit::TestCaller("twoGhostBodies", &BodyAABBTreeTest::twoGhostBodies));

    suite->addTest(new CppUnit::TestCaller("threeBodiesPairedAndRemove", &BodyAABBTreeTest::threeBodiesPairedAndRemove));

    return suite;
}
