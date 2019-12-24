#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "BodyAABBTreeTest.h"
using namespace urchin;

void BodyAABBTreeTest::twoBodiesPairedAndRemove()
{
    //add bodies test:
    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<WorkRigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyB = std::make_unique<WorkRigidBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA.get(), nullptr);
    bodyAabbTree.addBody(bodyB.get(), nullptr);
    auto *bodyANodeData = dynamic_cast<BodyAABBNodeData *>(bodyAabbTree.getNodeData(bodyA.get()));

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 1);
    AssertHelper::assertString(bodyAabbTree.getOverlappingPairs()[0]->getBody1()->getId(), "bodyB");
    AssertHelper::assertString(bodyAabbTree.getOverlappingPairs()[0]->getBody2()->getId(), "bodyA");
    AssertHelper::assertUnsignedInt(bodyANodeData->getOwnerPairContainers().size(), 0);

    //remove a body test:
    bodyAabbTree.removeBody(bodyB.get());
    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
}

void BodyAABBTreeTest::twoBodiesNotPaired()
{
    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<WorkRigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyB = std::make_unique<WorkRigidBody>("bodyB", PhysicsTransform(Point3<float>(10.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA.get(), nullptr);
    bodyAabbTree.addBody(bodyB.get(), nullptr);

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
}

void BodyAABBTreeTest::oneBodyWithAlternativePairAndRemoveIt()
{
    oneBodyWithAlternativePairAndRemove(true);
}

void BodyAABBTreeTest::oneBodyWithAlternativePairAndRemoveOther()
{
    oneBodyWithAlternativePairAndRemove(false);
}

void BodyAABBTreeTest::oneBodyWithAlternativePairAndRemove(bool removeBodyHavingAlternativePair)
{
    //add bodies test:
    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<WorkRigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyB = std::make_unique<WorkGhostBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA.get(), nullptr);
    bodyAabbTree.addBody(bodyB.get(), bodyB->getPairContainer());
    auto *bodyANodeData = dynamic_cast<BodyAABBNodeData *>(bodyAabbTree.getNodeData(bodyA.get()));
    auto *bodyBNodeData = dynamic_cast<BodyAABBNodeData *>(bodyAabbTree.getNodeData(bodyB.get()));

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
    std::vector<OverlappingPair> bodyBPairs = bodyB->getPairContainer()->retrieveCopyOverlappingPairs();
    AssertHelper::assertUnsignedInt(bodyBPairs.size(), 1);
    AssertHelper::assertString(bodyBPairs[0].getBody1()->getId(), "bodyB");
    AssertHelper::assertString(bodyBPairs[0].getBody2()->getId(), "bodyA");
    AssertHelper::assertUnsignedInt(bodyANodeData->getOwnerPairContainers().size(), 1);
    AssertHelper::assertUnsignedInt(bodyBNodeData->getOwnerPairContainers().size(), 0);

    //remove a body test:
    if(removeBodyHavingAlternativePair)
    {
        bodyAabbTree.removeBody(bodyB.get());
        AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
        AssertHelper::assertUnsignedInt(bodyANodeData->getOwnerPairContainers().size(), 0);
    } else
    {
        bodyAabbTree.removeBody(bodyA.get());
        AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 0);
    }
}

void BodyAABBTreeTest::twoBodiesWithAlternativePair()
{
    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<WorkGhostBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyB = std::make_unique<WorkGhostBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA.get(), bodyA->getPairContainer());
    bodyAabbTree.addBody(bodyB.get(), bodyB->getPairContainer());

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
    std::vector<OverlappingPair> bodyAPairs = bodyA->getPairContainer()->retrieveCopyOverlappingPairs();
    AssertHelper::assertUnsignedInt(bodyAPairs.size(), 1);
    AssertHelper::assertString(bodyAPairs[0].getBody1()->getId(), "bodyB");
    AssertHelper::assertString(bodyAPairs[0].getBody2()->getId(), "bodyA");
    std::vector<OverlappingPair> bodyBPairs = bodyB->getPairContainer()->retrieveCopyOverlappingPairs();
    AssertHelper::assertUnsignedInt(bodyBPairs.size(), 1);
    AssertHelper::assertString(bodyBPairs[0].getBody1()->getId(), "bodyB");
    AssertHelper::assertString(bodyBPairs[0].getBody2()->getId(), "bodyA");
}

void BodyAABBTreeTest::threeBodiesPairedAndRemove()
{
    //add bodies test:
    std::shared_ptr<CollisionBoxShape> cubeShape = std::make_shared<CollisionBoxShape>(Vector3<float>(0.5f, 0.5f, 0.5f));
    auto bodyA = std::make_unique<WorkRigidBody>("bodyA", PhysicsTransform(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyB = std::make_unique<WorkGhostBody>("bodyB", PhysicsTransform(Point3<float>(1.0f, 0.0f, 0.0f), Quaternion<float>()), cubeShape);
    auto bodyC = std::make_unique<WorkGhostBody>("bodyC", PhysicsTransform(Point3<float>(0.0f, 1.0f, 0.0f), Quaternion<float>()), cubeShape);
    BodyAABBTree bodyAabbTree;
    bodyAabbTree.addBody(bodyA.get(), nullptr);
    bodyAabbTree.addBody(bodyB.get(), bodyB->getPairContainer());
    bodyAabbTree.addBody(bodyC.get(), bodyC->getPairContainer());

    AssertHelper::assertUnsignedInt(bodyAabbTree.getOverlappingPairs().size(), 0);
    AssertHelper::assertUnsignedInt(bodyB->getPairContainer()->retrieveCopyOverlappingPairs().size(), 2);
    AssertHelper::assertUnsignedInt(bodyC->getPairContainer()->retrieveCopyOverlappingPairs().size(), 2);

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

CppUnit::Test *BodyAABBTreeTest::suite()
{
    auto *suite = new CppUnit::TestSuite("BodyAABBTreeTest");

    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("twoBodiesPairedAndRemove", &BodyAABBTreeTest::twoBodiesPairedAndRemove));
    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("twoBodiesNotPaired", &BodyAABBTreeTest::twoBodiesNotPaired));

    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("oneBodyWithAlternativePairAndRemoveIt", &BodyAABBTreeTest::oneBodyWithAlternativePairAndRemoveIt));
    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("oneBodyWithAlternativePairAndRemoveOther", &BodyAABBTreeTest::oneBodyWithAlternativePairAndRemoveOther));
    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("twoBodiesWithAlternativePair", &BodyAABBTreeTest::twoBodiesWithAlternativePair));

    suite->addTest(new CppUnit::TestCaller<BodyAABBTreeTest>("threeBodiesPairedAndRemove", &BodyAABBTreeTest::threeBodiesPairedAndRemove));

    return suite;
}
