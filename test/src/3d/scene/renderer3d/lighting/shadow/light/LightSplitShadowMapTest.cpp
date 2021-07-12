#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/renderer3d/lighting/shadow/light/LightSplitShadowMapTest.h>
#include <AssertHelper.h>
using namespace urchin;

void LightSplitShadowMapTest::modelsInFrustumSplit() {
    auto modelOctreeManager = buildModelOctreeManager({
        Point3<float>(1.0f, 2.0f, -3.0f),
        Point3<float>(1.0f, 2.0f, -10.0f)
    });
    auto light = std::make_unique<SunLight>(Vector3<float>(1.0f, 0.0f, 0.0f));
    auto lightShadowMap = std::make_unique<LightShadowMap>(light.get(), modelOctreeManager.get(), 300.0f, nullptr, 3, nullptr);
    auto lightSplitShadowMap = lightShadowMap->addLightSplitShadowMap();

    Frustum<float> frustumSplit(90.0f, 1.0f, 0.01f, 100.0f);
    lightSplitShadowMap.update(frustumSplit, false);

    AssertHelper::assertUnsignedInt(lightSplitShadowMap.getModels().size(), 2);
    AssertHelper::assertPoint3FloatEquals(lightSplitShadowMap.getShadowCasterReceiverBox().getMin(),
                                          Point3<float>(-10.5f, 1.5f, -101.0f) - LightSplitShadowMap::LIGHT_BOX_MARGIN,
                                          std::numeric_limits<float>::epsilon());
    AssertHelper::assertPoint3FloatEquals(lightSplitShadowMap.getShadowCasterReceiverBox().getMax(),
                                          Point3<float>(-2.5f, 2.5f, -1.5f) + LightSplitShadowMap::LIGHT_BOX_MARGIN,
                                          std::numeric_limits<float>::epsilon());
    cleanOctreeManager(modelOctreeManager);
}

void LightSplitShadowMapTest::modelsOutsideFrustumSplit() {
    auto modelOctreeManager = buildModelOctreeManager({
        Point3<float>(1.0f, 2.0f, -110.0f), //model not visible and doesn't produce shadow in the frustum split
        Point3<float>(-500.0f, 2.0f, -3.0f), //model not visible and too far to produce shadow in the frustum split
        Point3<float>(500.0f, 2.0f, -3.0f), //model not visible and in the wrong direction to produce shadow in the frustum split
    });
    auto light = std::make_unique<SunLight>(Vector3<float>(1.0f, 0.0f, 0.0f));
    auto lightShadowMap = std::make_unique<LightShadowMap>(light.get(), modelOctreeManager.get(), 300.0f, nullptr, 3, nullptr);
    auto lightSplitShadowMap = lightShadowMap->addLightSplitShadowMap();

    Frustum<float> frustumSplit(90.0f, 1.0f, 0.01f, 100.0f);
    lightSplitShadowMap.update(frustumSplit, false);

    AssertHelper::assertUnsignedInt(lightSplitShadowMap.getModels().size(), 0);
    AssertHelper::assertPoint3FloatEquals(lightSplitShadowMap.getShadowCasterReceiverBox().getMin(),
                                          Point3<float>(0.0f, 0.0f, 0.0f) - LightSplitShadowMap::LIGHT_BOX_MARGIN,
                                          std::numeric_limits<float>::epsilon());
    AssertHelper::assertPoint3FloatEquals(lightSplitShadowMap.getShadowCasterReceiverBox().getMax(),
                                          Point3<float>(0.0f, 0.0f, 0.0f) + LightSplitShadowMap::LIGHT_BOX_MARGIN,
                                          std::numeric_limits<float>::epsilon());
    cleanOctreeManager(modelOctreeManager);
}

void LightSplitShadowMapTest::modelOutsideFrustumProducingShadow() {
    auto modelOctreeManager = buildModelOctreeManager({
        Point3<float>(-250.0f, 2.0f, -3.0f), //model not visible but produces shadow in the frustum split
    });
    auto light = std::make_unique<SunLight>(Vector3<float>(1.0f, 0.0f, 0.0f));
    auto lightShadowMap = std::make_unique<LightShadowMap>(light.get(), modelOctreeManager.get(), 300.0f, nullptr, 3, nullptr);
    auto lightSplitShadowMap = lightShadowMap->addLightSplitShadowMap();

    Frustum<float> frustumSplit(90.0f, 1.0f, 0.01f, 100.0f);
    lightSplitShadowMap.update(frustumSplit, false);

    AssertHelper::assertUnsignedInt(lightSplitShadowMap.getModels().size(), 1);
    AssertHelper::assertPoint3FloatEquals(lightSplitShadowMap.getShadowCasterReceiverBox().getMin(),
                                          Point3<float>(-3.5f, 1.5f, -101.0f) - LightSplitShadowMap::LIGHT_BOX_MARGIN,
                                          std::numeric_limits<float>::epsilon());
    AssertHelper::assertPoint3FloatEquals(lightSplitShadowMap.getShadowCasterReceiverBox().getMax(),
                                          Point3<float>(-2.5f, 2.5f, 249.5f) + LightSplitShadowMap::LIGHT_BOX_MARGIN,
                                          std::numeric_limits<float>::epsilon());
    cleanOctreeManager(modelOctreeManager);
}

std::unique_ptr<OctreeManager<Model>> LightSplitShadowMapTest::buildModelOctreeManager(const std::vector<Point3<float>> &modelPositions) {
    auto modelOctreeManager = std::make_unique<OctreeManager<Model>>(10.0f);

    for (const auto& modelPosition : modelPositions) {
        auto model = new Model(""); //default model size: 1.0 x 1.0 x 1.0
        model->setPosition(modelPosition);
        modelOctreeManager->addOctreeable(model);
    }

    return modelOctreeManager;
}

void LightSplitShadowMapTest::cleanOctreeManager(const std::unique_ptr<OctreeManager<Model>>& modelOctreeManager) {
    for (auto& model : modelOctreeManager->getAllOctreeables()) {
        delete model;
    }
}

CppUnit::Test* LightSplitShadowMapTest::suite() {
    auto* suite = new CppUnit::TestSuite("ShadowManagerTest");

    suite->addTest(new CppUnit::TestCaller<LightSplitShadowMapTest>("modelsInFrustumSplit", &LightSplitShadowMapTest::modelsInFrustumSplit));
    suite->addTest(new CppUnit::TestCaller<LightSplitShadowMapTest>("modelsOutsideFrustumSplit", &LightSplitShadowMapTest::modelsOutsideFrustumSplit));
    suite->addTest(new CppUnit::TestCaller<LightSplitShadowMapTest>("modelOutsideFrustumProducingShadow", &LightSplitShadowMapTest::modelOutsideFrustumProducingShadow));

    return suite;
}
