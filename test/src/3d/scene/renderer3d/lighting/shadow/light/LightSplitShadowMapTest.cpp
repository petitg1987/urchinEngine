#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/renderer3d/lighting/shadow/light/LightSplitShadowMapTest.h>
#include <AssertHelper.h>
using namespace urchin;

void LightSplitShadowMapTest::modelsInFrustumSplit() {
    auto modelOcclusionCuller = buildModelOcclusionCuller({
        Point3<float>(1.0f, 2.0f, -3.0f),
        Point3<float>(1.0f, 2.0f, -10.0f)
    });
    auto light = std::make_unique<SunLight>(Vector3<float>(1.0f, 0.0f, 0.0f));
    auto shadowMapTexture = Texture::build("sm", 2048, 2048, TextureFormat::DEPTH_32_FLOAT);
    auto lightShadowMap = std::make_unique<LightShadowMap>(*light, *modelOcclusionCuller, 300.0f, 2048, 3);
    const auto &lightSplitShadowMap = lightShadowMap->getLightSplitShadowMaps()[0];

    SplitFrustum splitFrustum(Frustum(90.0f, 1.0f, 0.01f, 100.0f));
    lightSplitShadowMap->update(splitFrustum);

    AssertHelper::assertUnsignedIntEquals(lightSplitShadowMap->getModels().size(), 2);
}

void LightSplitShadowMapTest::modelsOutsideFrustumSplit() {
    auto modelOcclusionCuller = buildModelOcclusionCuller({
        Point3<float>(1.0f, 2.0f, -120.0f), //model not visible and doesn't produce shadow in the frustum split
        Point3<float>(-500.0f, 2.0f, -3.0f), //model not visible and too far to produce shadow in the frustum split
        Point3<float>(500.0f, 2.0f, -3.0f), //model not visible and in the wrong direction to produce shadow in the frustum split
    });
    auto light = std::make_unique<SunLight>(Vector3<float>(1.0f, 0.0f, 0.0f));
    auto shadowMapTexture = Texture::build("sm", 2048, 2048, TextureFormat::DEPTH_32_FLOAT);
    auto lightShadowMap = std::make_unique<LightShadowMap>(*light, *modelOcclusionCuller, 300.0f, 2048, 3);
    const auto &lightSplitShadowMap = lightShadowMap->getLightSplitShadowMaps()[0];

    SplitFrustum splitFrustum(Frustum(90.0f, 1.0f, 0.01f, 100.0f));
    lightSplitShadowMap->update(splitFrustum);

    AssertHelper::assertUnsignedIntEquals(lightSplitShadowMap->getModels().size(), 0);
}

void LightSplitShadowMapTest::modelOutsideFrustumProducingShadow() {
    auto modelOcclusionCuller = buildModelOcclusionCuller({
        Point3<float>(-250.0f, 2.0f, -3.0f), //model not visible but produces shadow in the frustum split
    });
    auto light = std::make_unique<SunLight>(Vector3<float>(1.0f, 0.0f, 0.0f));
    auto shadowMapTexture = Texture::build("sm", 2048, 2048, TextureFormat::DEPTH_32_FLOAT);
    auto lightShadowMap = std::make_unique<LightShadowMap>(*light, *modelOcclusionCuller, 300.0f, 2048, 3);
    const auto &lightSplitShadowMap = lightShadowMap->getLightSplitShadowMaps()[0];

    SplitFrustum splitFrustum(Frustum(90.0f, 1.0f, 0.01f, 100.0f));
    lightSplitShadowMap->update(splitFrustum);

    AssertHelper::assertUnsignedIntEquals(lightSplitShadowMap->getModels().size(), 1);
}

std::unique_ptr<ModelOcclusionCuller> LightSplitShadowMapTest::buildModelOcclusionCuller(const std::vector<Point3<float>> &modelPositions) {
    auto modelOcclusionCuller = std::make_unique<ModelOcclusionCuller>();

    for (const auto& modelPosition : modelPositions) {
        ModelBuilder modelBuilder("materials/opaque.uda");
        std::vector<Point3<float>> vertices = {Point3<float>(-0.5f, -0.5f, -0.5f), Point3<float>(0.5f, 0.5f, 0.5f), Point3<float>(0.5f, -0.5f, 0.5f)};
        std::vector<unsigned int> triangleIndices = {0, 1, 2};
        std::vector<Point2<float>> uvTexture = {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 0.0f)};
        std::unique_ptr<Model> model = modelBuilder.newModel("modelName", vertices, triangleIndices, uvTexture);

        model->setPosition(modelPosition);
        modelOcclusionCuller->addModel(std::move(model));
    }

    return modelOcclusionCuller;
}

CppUnit::Test* LightSplitShadowMapTest::suite() {
    auto* suite = new CppUnit::TestSuite("ShadowManagerTest");

    suite->addTest(new CppUnit::TestCaller<LightSplitShadowMapTest>("modelsInFrustumSplit", &LightSplitShadowMapTest::modelsInFrustumSplit));
    suite->addTest(new CppUnit::TestCaller<LightSplitShadowMapTest>("modelsOutsideFrustumSplit", &LightSplitShadowMapTest::modelsOutsideFrustumSplit));
    suite->addTest(new CppUnit::TestCaller<LightSplitShadowMapTest>("modelOutsideFrustumProducingShadow", &LightSplitShadowMapTest::modelOutsideFrustumProducingShadow));

    return suite;
}
