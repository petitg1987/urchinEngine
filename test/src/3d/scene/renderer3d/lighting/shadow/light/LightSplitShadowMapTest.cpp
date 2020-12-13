#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "LightSplitShadowMapTest.h"
#include "AssertHelper.h"
using namespace urchin;

void LightSplitShadowMapTest::modelInFrustumSplit() {
    auto modelOctreeManager = buildModelOctreeManager({Point3<float>(1.0f, 2.0f, -3.0f)});
    auto light = std::make_unique<SunLight>(Vector3<float>(1.0f, 0.0f, 0.0f));
    auto lightShadowMap = std::make_unique<LightShadowMap>(light.get(), modelOctreeManager.get(), 1);
    auto lightSplitShadowMap = lightShadowMap->addLightSplitShadowMap();

    Frustum<float> frustumSplit(90.0f, 1.0f, 0.01f, 100.0f);
    lightSplitShadowMap->update(frustumSplit, false);

    AssertHelper::assertUnsignedInt(lightSplitShadowMap->getModels().size(), 1);
    AABBox<float> result = lightSplitShadowMap->getShadowCasterReceiverBox();
    AssertHelper::assertPoint3FloatEquals(result.getMin(), Point3<float>(-3.5f, 1.5f, -101.0f));
    AssertHelper::assertPoint3FloatEquals(result.getMax(), Point3<float>(-2.5f, 2.5f, -1.5f));
    cleanOctreeManager(modelOctreeManager);
}

std::unique_ptr<OctreeManager<Model>> LightSplitShadowMapTest::buildModelOctreeManager(const std::vector<Point3<float>> &modelPositions) {
    auto modelOctreeManager = std::make_unique<OctreeManager<Model>>(9999.0);

    for(const auto& modelPosition : modelPositions) {
        auto model = new Model(""); //Default model size: 1.0 x 1.0 x 1.0
        model->setPosition(modelPosition);
        modelOctreeManager->addOctreeable(model);
    }

    return modelOctreeManager;
}

void LightSplitShadowMapTest::cleanOctreeManager(const std::unique_ptr<OctreeManager<Model>>& modelOctreeManager) {
    for(auto &model : modelOctreeManager->getAllOctreeables()) {
        delete model;
    }
}

CppUnit::Test* LightSplitShadowMapTest::suite() {
    auto* suite = new CppUnit::TestSuite("ShadowManagerTest");

    suite->addTest(new CppUnit::TestCaller<LightSplitShadowMapTest>("test", &LightSplitShadowMapTest::test));

    return suite;
}
