#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/collision/broadphase/aabbtree/BodyAABBTreeTest.h>
using namespace urchin;

void BodyAABBTreeTest::twoBodiesPairedAndRemove() {
    //add bodies test:
    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<RigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyB = std::make_unique<RigidBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA.get());
    bodyAabbTree.addBody(bodyB.get());
    auto* bodyANodeData = dynamic_cast<BodyAABBNodeData*>(bodyAabbTree.getNodeData(bodyA.get()));

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 1);
    AssertHelper::assertString(bodyAabbTree.getOverlappingPairs()[0]->getBody1()->getId(), "bodyB");
    AssertHelper::assertString(bodyAabbTree.getOverlappingPairs()[0]->getBody2()->getId(), "bodyA");
    AssertHelper::assertUnsignedInt(bodyANodeData->getOwnerPairContainers().size(), 0);

    //remove a body test:
    bodyAabbTree.removeBody(bodyB.get());
    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
}

void BodyAABBTreeTest::twoBodiesNotPaired() {
    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<RigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyB = std::make_unique<RigidBody>("bodyB", PhysicsTransform(Point3<float>(10.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA.get());
    bodyAabbTree.addBody(bodyB.get());

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
    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<RigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyB = std::make_unique<GhostBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA.get());
    bodyAabbTree.addBody(bodyB.get());
    auto* bodyANodeData = dynamic_cast<BodyAABBNodeData*>(bodyAabbTree.getNodeData(bodyA.get()));
    auto* bodyBNodeData = dynamic_cast<BodyAABBNodeData*>(bodyAabbTree.getNodeData(bodyB.get()));

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
    std::vector<OverlappingPair> bodyBPairs = bodyB->getPairContainer()->retrieveCopyOverlappingPairs();
    AssertHelper::assertUnsignedInt(bodyBPairs.size(), 1);
    AssertHelper::assertString(bodyBPairs[0].getBody1()->getId(), "bodyB");
    AssertHelper::assertString(bodyBPairs[0].getBody2()->getId(), "bodyA");
    AssertHelper::assertUnsignedInt(bodyANodeData->getOwnerPairContainers().size(), 1);
    AssertHelper::assertUnsignedInt(bodyBNodeData->getOwnerPairContainers().size(), 0);

    //remove a body test:
    if (removeGhostBody) {
        bodyAabbTree.removeBody(bodyB.get());
        AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
        AssertHelper::assertUnsignedInt(bodyANodeData->getOwnerPairContainers().size(), 0);
    } else {
        bodyAabbTree.removeBody(bodyA.get());
        AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
    }
}

void BodyAABBTreeTest::twoGhostBodies() {
    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<GhostBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyB = std::make_unique<GhostBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA.get());
    bodyAabbTree.addBody(bodyB.get());

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
    std::vector<OverlappingPair> bodyAPairs = bodyA->getPairContainer()->retrieveCopyOverlappingPairs();
    AssertHelper::assertUnsignedInt(bodyAPairs.size(), 0); //ghost bodies do not see each other
    std::vector<OverlappingPair> bodyBPairs = bodyB->getPairContainer()->retrieveCopyOverlappingPairs();
    AssertHelper::assertUnsignedInt(bodyBPairs.size(), 0); //ghost bodies do not see each other
}

void BodyAABBTreeTest::threeBodiesPairedAndRemove() {
    //add bodies test:
    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<RigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyB = std::make_unique<GhostBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyC = std::make_unique<GhostBody>("bodyC", PhysicsTransform(Point3<float>(0.0f, 1.0f, 0.0f), Quaternion<float>()), cubeShape);
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA.get());
    bodyAabbTree.addBody(bodyB.get());
    bodyAabbTree.addBody(bodyC.get());

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
    AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 1);
    AssertHelper::assertObject(bodyB->getPairContainer()->retrieveCopyOverlappingPairs()[0].getBody2(), bodyA.get());
    AssertHelper::assertUnsignedInt(bodyC->getPairContainer()->retrieveCopyOverlappingPairs().size(), 1);
    AssertHelper::assertObject(bodyC->getPairContainer()->retrieveCopyOverlappingPairs()[0].getBody2(), bodyA.get());

    //remove a body test:
    bodyAabbTree.removeBody(bodyB.get());
    AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
    AssertHelper::assertUnsignedInt(bodyC->getPairContainer()->retrieveCopyOverlappingPairs().size(), 1);
    bodyAabbTree.removeBody(bodyA.get());
    AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
    AssertHelper::assertUnsignedInt(bodyC->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
    bodyAabbTree.removeBody(bodyC.get());
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
