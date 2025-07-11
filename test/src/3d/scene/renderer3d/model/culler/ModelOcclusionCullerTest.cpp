#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/renderer3d/model/culler/ModelOcclusionCullerTest.h>
#include <AssertHelper.h>
using namespace urchin;

void ModelOcclusionCullerTest::movingModel() {
    auto modelOcclusionCuller = std::make_unique<ModelOcclusionCuller>();
    std::shared_ptr model = buildModel(Point3(-500.0f, 2.0f, -3.0f)); //model outside frustum
    modelOcclusionCuller->addModel(model);
    Frustum frustum(90.0f, 1.0f, 0.01f, 100.0f);

    std::vector<Model*> models;
    modelOcclusionCuller->refresh();
    modelOcclusionCuller->getModelsInConvexObject(frustum, models);
    modelOcclusionCuller->postRefresh();
    AssertHelper::assertUnsignedIntEquals(models.size(), 0);

    model->setTransform(Transform(Point3(0.0f, 2.0f, -3.0f), Quaternion<float>())); //model inside frustum
    models.clear();
    modelOcclusionCuller->refresh();
    modelOcclusionCuller->getModelsInConvexObject(frustum, models);
    modelOcclusionCuller->postRefresh();
    AssertHelper::assertUnsignedIntEquals(models.size(), 1);
}

void ModelOcclusionCullerTest::updateCullBehavior() {
    auto modelOcclusionCuller = std::make_unique<ModelOcclusionCuller>();
    std::shared_ptr model = buildModel(Point3(-500.0f, 2.0f, -3.0f)); //model outside frustum
    modelOcclusionCuller->addModel(model);
    Frustum frustum(90.0f, 1.0f, 0.01f, 100.0f);

    std::vector<Model*> models;
    modelOcclusionCuller->refresh();
    modelOcclusionCuller->getModelsInConvexObject(frustum, models);
    modelOcclusionCuller->postRefresh();
    AssertHelper::assertUnsignedIntEquals(models.size(), 0);

    model->setCullBehavior(Model::CullBehavior::NO_CULL);
    models.clear();
    modelOcclusionCuller->refresh();
    modelOcclusionCuller->getModelsInConvexObject(frustum, models);
    modelOcclusionCuller->postRefresh();
    AssertHelper::assertUnsignedIntEquals(models.size(), 1);

    model->setCullBehavior(Model::CullBehavior::CULL);
    models.clear();
    modelOcclusionCuller->refresh();
    modelOcclusionCuller->getModelsInConvexObject(frustum, models);
    modelOcclusionCuller->postRefresh();
    AssertHelper::assertUnsignedIntEquals(models.size(), 0);
}

std::unique_ptr<Model> ModelOcclusionCullerTest::buildModel(const Point3<float>& modelPosition) const {
    ModelBuilder modelBuilder("materials/opaque.uda");

    std::vector vertices = {Point3(-0.5f, -0.5f, -0.5f), Point3(0.5f, 0.5f, 0.5f), Point3(0.5f, -0.5f, 0.5f)};
    std::vector<std::array<uint32_t, 3>> triangleIndices = {{0u, 1u, 2u}};
    std::vector uvTexture = {Point2(0.0f, 0.0f), Point2(0.0f, 0.0f), Point2(0.0f, 0.0f)};

    std::unique_ptr<Model> model = modelBuilder.newModel("modelName", vertices, triangleIndices, uvTexture);
    model->setPosition(modelPosition);

    return model;
}

CppUnit::Test* ModelOcclusionCullerTest::suite() {
    auto* suite = new CppUnit::TestSuite("ModelOcclusionCullerTest");

    suite->addTest(new CppUnit::TestCaller("movingModel", &ModelOcclusionCullerTest::movingModel));
    suite->addTest(new CppUnit::TestCaller("updateCullBehavior", &ModelOcclusionCullerTest::updateCullBehavior));

    return suite;
}
