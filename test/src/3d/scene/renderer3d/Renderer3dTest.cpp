#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/renderer3d/Renderer3dTest.h>
#include <AssertHelper.h>
using namespace urchin;

void Renderer3dTest::modelPreWarm() {
    NullRenderTarget nullRenderTarget(1920, 1080);
    I18nService nullI18nService;
    Renderer3d renderer3D(nullRenderTarget, nullI18nService);
    renderer3D.setCamera(std::make_shared<FreeCamera>(90.0f, 0.01f, 1000.0f));
    std::shared_ptr<Model> opaqueModel = buildModel("resources/materials/opaque.uda", "model1", Point3<float>(0.0f, 0.0f, -10.0f));
    std::shared_ptr<Model> transparentModel = buildModel("resources/materials/transparent.uda", "model2", Point3<float>(0.0f, 0.0f, -10.0f));
    std::shared_ptr<Model> notVisibleModel = buildModel("resources/materials/opaque.uda", "model3", Point3<float>(0.0f, 0.0f, 25.0f));
    renderer3D.addModel(opaqueModel);
    renderer3D.addModel(transparentModel);
    renderer3D.addModel(notVisibleModel);

    renderer3D.preWarmModels();

    const ModelSetDisplayer& mainModelsDisplayer = renderer3D.getModelSetDisplayer();
    AssertHelper::assertUnsignedIntEquals(mainModelsDisplayer.getModels().size(), 1);
    AssertHelper::assertStringEquals(mainModelsDisplayer.getModels()[0]->getConstMeshes()->getName(), opaqueModel->getConstMeshes()->getName());
    AssertHelper::assertTrue(mainModelsDisplayer.isModelDisplayerExist(*opaqueModel));
    const ModelSetDisplayer& transparentModelsDisplayer = renderer3D.getTransparentManager().getModelSetDisplayer();
    AssertHelper::assertUnsignedIntEquals(transparentModelsDisplayer.getModels().size(), 1);
    AssertHelper::assertStringEquals(transparentModelsDisplayer.getModels()[0]->getConstMeshes()->getName(), transparentModel->getConstMeshes()->getName());
    AssertHelper::assertTrue(transparentModelsDisplayer.isModelDisplayerExist(*transparentModel));
}

std::unique_ptr<Model> Renderer3dTest::buildModel(std::string materialFilename, const std::string& modelName, const Point3<float>& position) const {
    ModelBuilder modelBuilder(std::move(materialFilename));

    std::vector<Point3<float>> vertices = {Point3<float>(-0.01f, 0.0f, 0.0f), Point3<float>(0.0f, 0.01f, 0.0f), Point3<float>(0.01f, 0.0f, 0.0f)};
    std::vector<unsigned int> triangleIndices = {0, 1, 2};
    std::vector<Point2<float>> uv = {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    std::unique_ptr<Model> model = modelBuilder.newModel(modelName, vertices, triangleIndices, uv);
    model->setPosition(position);

    return model;
}

CppUnit::Test* Renderer3dTest::suite() {
    auto* suite = new CppUnit::TestSuite("Renderer3dTest");

    suite->addTest(new CppUnit::TestCaller<Renderer3dTest>("modelPreWarm", &Renderer3dTest::modelPreWarm));

    return suite;
}

