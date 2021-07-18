#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <AssertHelper.h>
#include <physics/collision/bodystate/IslandContainerTest.h>
using namespace urchin;

/**
 * Create 4 bodies. Body n will be in contact with body n+1.
 * All bodies of container should belong to same island.
 */
void IslandContainerTest::cascadeMergeIslands() {
    std::array<std::unique_ptr<TestBody>, 4> bodies = {std::make_unique<TestBody>(), std::make_unique<TestBody>(), std::make_unique<TestBody>(), std::make_unique<TestBody>()};
    std::vector<IslandElement*> bodiesPtr;
    bodiesPtr.reserve(bodies.size());
    for (auto& body : bodies) {
        bodiesPtr.emplace_back(body.get());
    }

    IslandContainer islandContainer;
    islandContainer.reset(bodiesPtr);
    islandContainer.mergeIsland(*bodies[0], *bodies[1]); //body 0 is in contact with body 1
    islandContainer.mergeIsland(*bodies[1], *bodies[2]); //body 1 is in contact with body 2
    islandContainer.mergeIsland(*bodies[2], *bodies[3]); //body 2 is in contact with body 3
    const std::vector<IslandElementLink>& islandElementsLink = islandContainer.retrieveSortedIslandElements();

    AssertHelper::assertUnsignedInt(islandElementsLink.size(), 4);
    unsigned int islandId = islandElementsLink[0].islandIdRef;
    for (std::size_t i = 1; i < islandElementsLink.size(); ++i) {
        AssertHelper::assertUnsignedInt(islandElementsLink[i].islandIdRef, islandId);
    }
}

/**
 * Create 3 bodies. Each body will be in contact with all others bodies
 * All bodies of container should belong to same island.
 */
void IslandContainerTest::mergeAllIslands() {
    std::array<std::unique_ptr<TestBody>, 3> bodies = {std::make_unique<TestBody>(), std::make_unique<TestBody>(), std::make_unique<TestBody>()};
    std::vector<IslandElement*> bodiesPtr;
    bodiesPtr.reserve(bodies.size());
    for (auto& body : bodies) {
        bodiesPtr.emplace_back(body.get());
    }

    IslandContainer islandContainer;
    islandContainer.reset(bodiesPtr);
    islandContainer.mergeIsland(*bodies[0], *bodies[1]); //body 0 is in contact with body 1
    islandContainer.mergeIsland(*bodies[0], *bodies[2]); //body 0 is in contact with body 2
    islandContainer.mergeIsland(*bodies[1], *bodies[2]); //body 1 is in contact with body 2
    const std::vector<IslandElementLink>& islandElementsLink = islandContainer.retrieveSortedIslandElements();

    AssertHelper::assertUnsignedInt(islandElementsLink.size(), 3);
    unsigned int islandId = islandElementsLink[0].islandIdRef;
    for (std::size_t i = 1; i < islandElementsLink.size(); ++i) {
        AssertHelper::assertUnsignedInt(islandElementsLink[i].islandIdRef, islandId);
    }
}

/**
 * Create 4 bodies. Body 0 is in contact with body 3. Body 2 is in contact with body 1.
 * We should have two distinct islands.
 */
void IslandContainerTest::createTwoSeparateIslands() {
    std::array<std::unique_ptr<TestBody>, 4> bodies = {std::make_unique<TestBody>(), std::make_unique<TestBody>(), std::make_unique<TestBody>(), std::make_unique<TestBody>()};
    std::vector<IslandElement*> bodiesPtr;
    bodiesPtr.reserve(bodies.size());
    for (auto& body : bodies) {
        bodiesPtr.emplace_back(body.get());
    }

    IslandContainer islandContainer;
    islandContainer.reset(bodiesPtr);
    islandContainer.mergeIsland(*bodies[0], *bodies[3]); //body 0 is in contact with body 3
    islandContainer.mergeIsland(*bodies[2], *bodies[1]); //body 2 is in contact with body 1
    const std::vector<IslandElementLink>& islandElementsLink = islandContainer.retrieveSortedIslandElements();

    AssertHelper::assertUnsignedInt(islandElementsLink.size(), 4);
    AssertHelper::assertUnsignedInt(islandElementsLink[0].islandIdRef, islandElementsLink[1].islandIdRef);
    AssertHelper::assertUnsignedInt(islandElementsLink[2].islandIdRef, islandElementsLink[3].islandIdRef);
    AssertHelper::assertTrue(islandElementsLink[0].islandIdRef != islandElementsLink[2].islandIdRef);
}

CppUnit::Test* IslandContainerTest::suite() {
    auto* suite = new CppUnit::TestSuite("IslandContainerTest");

    suite->addTest(new CppUnit::TestCaller<IslandContainerTest>("cascadeMergeIslands", &IslandContainerTest::cascadeMergeIslands));
    suite->addTest(new CppUnit::TestCaller<IslandContainerTest>("mergeAllIslands", &IslandContainerTest::mergeAllIslands));
    suite->addTest(new CppUnit::TestCaller<IslandContainerTest>("createTwoSeparateIslands", &IslandContainerTest::createTwoSeparateIslands));

    return suite;
}
