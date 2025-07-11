#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/renderer3d/Renderer3dTest.h>
#include <AssertHelper.h>
using namespace urchin;

void Renderer3dTest::modelPreWarm() {
    OffscreenRender renderTarget("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    renderTarget.setOutputSize(1920, 1080, 1, false);
    I18nService nullI18nService;
    auto camera = std::make_unique<FreeCamera>(90.0f, 0.01f, 1000.0f);
    Renderer3d renderer3D(1.0f, renderTarget, std::move(camera), VisualConfig(), nullI18nService);
    std::shared_ptr opaqueModel = buildModel("materials/opaque.uda", "model1", Point3(0.0f, 0.0f, -10.0f));
    std::shared_ptr transparentModel = buildModel("materials/transparent.uda", "model2", Point3(0.0f, 0.0f, -10.0f));
    std::shared_ptr notVisibleModel = buildModel("materials/opaque.uda", "model3", Point3(0.0f, 0.0f, 25.0f));
    renderer3D.addModel(opaqueModel);
    renderer3D.addModel(transparentModel);
    renderer3D.addModel(notVisibleModel);

    renderer3D.preWarmModels();

    const ModelSetDisplayer& mainModelsDisplayer = renderer3D.getModelSetDisplayer();
    AssertHelper::assertUnsignedIntEquals(mainModelsDisplayer.getModels().size(), 1);
    AssertHelper::assertStringEquals(mainModelsDisplayer.getModels()[0]->getConstMeshes()->getName(), opaqueModel->getConstMeshes()->getName());
    AssertHelper::assertTrue(mainModelsDisplayer.isDisplayerExist(*opaqueModel));
    const ModelSetDisplayer& transparentModelsDisplayer = renderer3D.getTransparentManager().getModelSetDisplayer();
    AssertHelper::assertUnsignedIntEquals(transparentModelsDisplayer.getModels().size(), 1);
    AssertHelper::assertStringEquals(transparentModelsDisplayer.getModels()[0]->getConstMeshes()->getName(), transparentModel->getConstMeshes()->getName());
    AssertHelper::assertTrue(transparentModelsDisplayer.isDisplayerExist(*transparentModel));
}

std::unique_ptr<Model> Renderer3dTest::buildModel(const std::string& materialFilename, const std::string& modelName, const Point3<float>& position) const {
    ModelBuilder modelBuilder(materialFilename);

    std::vector vertices = {Point3(-0.01f, 0.0f, 0.0f), Point3(0.0f, 0.01f, 0.0f), Point3(0.01f, 0.0f, 0.0f)};
    std::vector<std::array<uint32_t, 3>> triangleIndices = {{0u, 1u, 2u}};
    std::vector uvTexture = {Point2(0.0f, 0.0f), Point2(0.0f, 0.0f), Point2(0.0f, 0.0f)};

    std::unique_ptr<Model> model = modelBuilder.newModel(modelName, vertices, triangleIndices, uvTexture);
    model->setPosition(position);

    return model;
}

CppUnit::Test* Renderer3dTest::suite() {
    auto* suite = new CppUnit::TestSuite("Renderer3dTest");

    suite->addTest(new CppUnit::TestCaller("modelPreWarm", &Renderer3dTest::modelPreWarm));

    return suite;
}

