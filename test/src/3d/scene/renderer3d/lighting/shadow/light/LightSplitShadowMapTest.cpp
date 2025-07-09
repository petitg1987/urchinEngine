#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/renderer3d/lighting/shadow/light/LightSplitShadowMapTest.h>
#include <AssertHelper.h>
using namespace urchin;

void LightSplitShadowMapTest::modelsInFrustumSplit() {
    auto modelOcclusionCuller = buildModelOcclusionCuller({
        Point3(1.0f, 2.0f, -3.0f),
        Point3(1.0f, 2.0f, -10.0f)
    });
    auto light = std::make_unique<SunLight>(Vector3(1.0f, 0.0f, 0.0f));
    auto shadowMapTexture = Texture::build("sm", 2048, 2048, TextureFormat::DEPTH_32_FLOAT);
    auto lightShadowMap = std::make_unique<LightShadowMap>(true, *light, *modelOcclusionCuller, 300.0f, 2048, 3);
    const auto &lightSplitShadowMap = lightShadowMap->getLightSplitShadowMaps()[0];

    SplitFrustum splitFrustum(Frustum(90.0f, 1.0f, 0.01f, 100.0f));
    lightSplitShadowMap->onSplitFrustumUpdated(splitFrustum);
    lightSplitShadowMap->updateVisibleModels();

    AssertHelper::assertUnsignedIntEquals(lightSplitShadowMap->getModels().size(), 2);
}

void LightSplitShadowMapTest::modelsOutsideFrustumSplit() {
    auto modelOcclusionCuller = buildModelOcclusionCuller({
        Point3(1.0f, 2.0f, -250.0f), //model not visible and doesn't produce shadow in the frustum split
        Point3(-500.0f, 2.0f, -3.0f), //model not visible and too far to produce shadow in the frustum split
        Point3(500.0f, 2.0f, -3.0f), //model not visible and in the wrong direction to produce shadow in the frustum split
    });
    auto light = std::make_unique<SunLight>(Vector3(1.0f, 0.0f, 0.0f));
    auto shadowMapTexture = Texture::build("sm", 2048, 2048, TextureFormat::DEPTH_32_FLOAT);
    auto lightShadowMap = std::make_unique<LightShadowMap>(true, *light, *modelOcclusionCuller, 300.0f, 2048, 3);
    const auto &lightSplitShadowMap = lightShadowMap->getLightSplitShadowMaps()[0];

    SplitFrustum splitFrustum(Frustum(90.0f, 1.0f, 0.01f, 100.0f));
    lightSplitShadowMap->onSplitFrustumUpdated(splitFrustum);
    lightSplitShadowMap->updateVisibleModels();

    AssertHelper::assertUnsignedIntEquals(lightSplitShadowMap->getModels().size(), 0);
}

void LightSplitShadowMapTest::modelOutsideFrustumProducingShadow() {
    auto modelOcclusionCuller = buildModelOcclusionCuller({
        Point3(-250.0f, 2.0f, -3.0f), //model not visible but produces shadow in the frustum split
        Point3(1.0f, 2.0f, -150.0f), //model after frustum far plane but still in frustum bounding sphere
    });
    auto light = std::make_unique<SunLight>(Vector3(1.0f, 0.0f, 0.0f));
    auto shadowMapTexture = Texture::build("sm", 2048, 2048, TextureFormat::DEPTH_32_FLOAT);
    auto lightShadowMap = std::make_unique<LightShadowMap>(true, *light, *modelOcclusionCuller, 300.0f, 2048, 3);
    const auto &lightSplitShadowMap = lightShadowMap->getLightSplitShadowMaps()[0];

    SplitFrustum splitFrustum(Frustum(90.0f, 1.0f, 0.01f, 100.0f));
    lightSplitShadowMap->onSplitFrustumUpdated(splitFrustum);
    lightSplitShadowMap->updateVisibleModels();

    AssertHelper::assertUnsignedIntEquals(lightSplitShadowMap->getModels().size(), 2);
}

std::unique_ptr<ModelOcclusionCuller> LightSplitShadowMapTest::buildModelOcclusionCuller(const std::vector<Point3<float>> &modelPositions) {
    auto modelOcclusionCuller = std::make_unique<ModelOcclusionCuller>();

    for (const auto& modelPosition : modelPositions) {
        ModelBuilder modelBuilder("materials/opaque.uda");
        std::vector vertices = {Point3(-0.5f, -0.5f, -0.5f), Point3(0.5f, 0.5f, 0.5f), Point3(0.5f, -0.5f, 0.5f)};
        std::vector triangleIndices = {0u, 1u, 2u};
        std::vector uvTexture = {Point2(0.0f, 0.0f), Point2(0.0f, 0.0f), Point2(0.0f, 0.0f)};
        std::unique_ptr<Model> model = modelBuilder.newModel("modelName", vertices, triangleIndices, uvTexture);

        model->setPosition(modelPosition);
        modelOcclusionCuller->addModel(std::move(model));
    }

    return modelOcclusionCuller;
}

CppUnit::Test* LightSplitShadowMapTest::suite() {
    auto* suite = new CppUnit::TestSuite("ShadowManagerTest");

    suite->addTest(new CppUnit::TestCaller("modelsInFrustumSplit", &LightSplitShadowMapTest::modelsInFrustumSplit));
    suite->addTest(new CppUnit::TestCaller("modelsOutsideFrustumSplit", &LightSplitShadowMapTest::modelsOutsideFrustumSplit));
    suite->addTest(new CppUnit::TestCaller("modelOutsideFrustumProducingShadow", &LightSplitShadowMapTest::modelOutsideFrustumProducingShadow));

    return suite;
}
