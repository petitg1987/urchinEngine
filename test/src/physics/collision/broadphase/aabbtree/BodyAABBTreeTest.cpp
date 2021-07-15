#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/collision/broadphase/aabbtree/BodyAABBTreeTest.h>
using namespace urchin;

void BodyAABBTreeTest::twoBodiesPairedAndRemove() {
    //add bodies test:
    std::unique_ptr<CollisionBoxShape> cubeShapeA = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<RigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeA));
    std::unique_ptr<CollisionBoxShape> cubeShapeB = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyB = std::make_unique<RigidBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeB));
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(*bodyA);
    bodyAabbTree.addBody(*bodyB);
    auto bodyANodeData = dynamic_cast<BodyAABBNodeData&>(bodyAabbTree.getNodeData(bodyA.get()));

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 1);
    AssertHelper::assertString(bodyAabbTree.getOverlappingPairs()[0]->getBody1().getId(), "bodyB");
    AssertHelper::assertString(bodyAabbTree.getOverlappingPairs()[0]->getBody2().getId(), "bodyA");
    AssertHelper::assertUnsignedInt(bodyANodeData.getOwnerPairContainers().size(), 0);

    //remove a body test:
    bodyAabbTree.removeBody(*bodyB);
    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
}

void BodyAABBTreeTest::twoBodiesNotPaired() {
    std::unique_ptr<CollisionBoxShape> cubeShapeA = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<RigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeA));
    std::unique_ptr<CollisionBoxShape> cubeShapeB = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyB = std::make_unique<RigidBody>("bodyB", PhysicsTransform(Point3<float>(10.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeB));
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(*bodyA);
    bodyAabbTree.addBody(*bodyB);

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
}

void BodyAABBTreeTest::oneGhostBodyAndRemoveIt() {
    oneGhostBodyAndRemove(true);
}

void BodyAABBTreeTest::oneGhostBodyAndRemoveOther() {
    oneGhostBodyAndRemove(false);
}

void BodyAABBTreeTest::oneGhostBodyAndRemove(bool removeGhostBody) {
    //add bodies test:
    std::unique_ptr<CollisionBoxShape> cubeShapeA = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<RigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeA));
    std::unique_ptr<CollisionBoxShape> cubeShapeB = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyB = std::make_unique<GhostBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeB));
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(*bodyA);
    bodyAabbTree.addBody(*bodyB);
    auto bodyANodeData = dynamic_cast<BodyAABBNodeData&>(bodyAabbTree.getNodeData(bodyA.get()));
    auto bodyBNodeData = dynamic_cast<BodyAABBNodeData&>(bodyAabbTree.getNodeData(bodyB.get()));

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
    std::vector<OverlappingPair> bodyBPairs = bodyB->getPairContainer()->retrieveCopyOverlappingPairs();
    AssertHelper::assertUnsignedInt(bodyBPairs.size(), 1);
    AssertHelper::assertString(bodyBPairs[0].getBody1().getId(), "bodyB");
    AssertHelper::assertString(bodyBPairs[0].getBody2().getId(), "bodyA");
    AssertHelper::assertUnsignedInt(bodyANodeData.getOwnerPairContainers().size(), 1);
    AssertHelper::assertUnsignedInt(bodyBNodeData.getOwnerPairContainers().size(), 0);

    //remove a body test:
    if (removeGhostBody) {
        bodyAabbTree.removeBody(*bodyB);
        auto bodyANodeDataAfterRemove = dynamic_cast<BodyAABBNodeData&>(bodyAabbTree.getNodeData(bodyA.get()));
        AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
        AssertHelper::assertUnsignedInt(bodyANodeDataAfterRemove.getOwnerPairContainers().size(), 0);
    } else {
        bodyAabbTree.removeBody(*bodyA);
        AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
    }
}

void BodyAABBTreeTest::twoGhostBodies() {
    std::unique_ptr<CollisionBoxShape> cubeShapeA = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<GhostBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeA));
    std::unique_ptr<CollisionBoxShape> cubeShapeB = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyB = std::make_unique<GhostBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeB));
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(*bodyA);
    bodyAabbTree.addBody(*bodyB);

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
    std::vector<OverlappingPair> bodyAPairs = bodyA->getPairContainer()->retrieveCopyOverlappingPairs();
    AssertHelper::assertUnsignedInt(bodyAPairs.size(), 0); //ghost bodies do not see each other
    std::vector<OverlappingPair> bodyBPairs = bodyB->getPairContainer()->retrieveCopyOverlappingPairs();
    AssertHelper::assertUnsignedInt(bodyBPairs.size(), 0); //ghost bodies do not see each other
}

void BodyAABBTreeTest::threeBodiesPairedAndRemove() {
    //add bodies test:
    std::unique_ptr<CollisionBoxShape> cubeShapeA = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<RigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeA));
    std::unique_ptr<CollisionBoxShape> cubeShapeB = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyB = std::make_unique<GhostBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeB));
    std::unique_ptr<CollisionBoxShape> cubeShapeC = std::make_unique<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyC = std::make_unique<GhostBody>("bodyC", PhysicsTransform(Point3<float>(0.0f, 1.0f, 0.0f), Quaternion<float>()), std::move(cubeShapeC));
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(*bodyA);
    bodyAabbTree.addBody(*bodyB);
    bodyAabbTree.addBody(*bodyC);

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
    AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 1);
    AssertHelper::assertObject(&bodyB->getPairContainer()->retrieveCopyOverlappingPairs()[0].getBody2(), bodyA.get());
    AssertHelper::assertUnsignedInt(bodyC->getPairContainer()->retrieveCopyOverlappingPairs().size(), 1);
    AssertHelper::assertObject(&bodyC->getPairContainer()->retrieveCopyOverlappingPairs()[0].getBody2(), bodyA.get());

    //remove a body test:
    bodyAabbTree.removeBody(*bodyB);
    AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
    AssertHelper::assertUnsignedInt(bodyC->getPairContainer()->retrieveCopyOverlappingPairs().size(), 1);
    bodyAabbTree.removeBody(*bodyA);
    AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
    AssertHelper::assertUnsignedInt(bodyC->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
    bodyAabbTree.removeBody(*bodyC);
    AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
    AssertHelper::assertUnsignedInt(bodyC->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
}

CppUnit::Test* BodyAABBTreeTest::suite() {
    auto* suite = new CppUnit::TestSuite("BodyAABBTreeTest");

    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("twoBodiesPairedAndRemove", &BodyAABBTreeTest::twoBodiesPairedAndRemove));
    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("twoBodiesNotPaired", &BodyAABBTreeTest::twoBodiesNotPaired));

    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("oneGhostBodyAndRemoveIt", &BodyAABBTreeTest::oneGhostBodyAndRemoveIt));
    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("oneGhostBodyAndRemoveOther", &BodyAABBTreeTest::oneGhostBodyAndRemoveOther));
    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("twoGhostBodies", &BodyAABBTreeTest::twoGhostBodies));

    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("threeBodiesPairedAndRemove", &BodyAABBTreeTest::threeBodiesPairedAndRemove));

    return suite;
}
