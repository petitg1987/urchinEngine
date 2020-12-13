#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "LightSplitShadowMapTest.h"
#include "AssertHelper.h"
using namespace urchin;

void LightSplitShadowMapTest::test() { //TODO impl
    auto modelOctreeManager = buildModelOctreeManager({Point3<float>(5.0, 2.0, 3.0)});
    auto light = std::make_unique<SunLight>(Vector3<float>(1.0f, 0.0f, 0.0f));
    auto lightShadowMap = std::make_unique<LightShadowMap>(light.get(), modelOctreeManager.get(), 1);
    auto lightSplitShadowMap = std::make_unique<LightSplitShadowMap>(lightShadowMap.get());

    lightSplitShadowMap->update(Frustum<float>(90.0f, 1.0f, 0.01f, 1000.0f), false);

    AABBox<float> result = lightSplitShadowMap->getShadowCasterReceiverBox();
    AssertHelper::assertPoint3FloatEquals(result.getMin(), Point3<float>(2.5f, 1.5f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(result.getMax(), Point3<float>(3.5f, 2.5f, 0.0f));
}

std::unique_ptr<OctreeManager<Model>> LightSplitShadowMapTest::buildModelOctreeManager(const std::vector<Point3<float>> &modelPositions) {
    auto modelOctreeManager = std::make_unique<OctreeManager<Model>>(9999.0);

    for(const auto& modelPosition : modelPositions) {
        auto modelInLightView = std::make_unique<Model>(""); //Default model size: 1.0x1.0x1.0
        modelInLightView->setPosition(modelPosition);

        modelOctreeManager->addOctreeable(modelInLightView.get());
    }

    return modelOctreeManager;
}

CppUnit::Test* LightSplitShadowMapTest::suite() {
    auto* suite = new CppUnit::TestSuite("ShadowManagerTest");

    suite->addTest(new CppUnit::TestCaller<LightSplitShadowMapTest>("test", &LightSplitShadowMapTest::test));

    return suite;
}
