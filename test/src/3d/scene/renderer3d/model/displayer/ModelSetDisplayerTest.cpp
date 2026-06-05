#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <Urchin3dEngine.h>

#include "3d/scene/renderer3d/model/displayer/ModelSetDisplayerTest.h"
#include "AssertHelper.h"
using namespace urchin;

void ModelSetDisplayerTest::updateInstancedModel() {
    auto model1 = ModelBuilder().newEmptyModel("testModel1");
    auto model2 = ModelBuilder().newEmptyModel("testModel2");
    auto model3 = std::make_unique<Model>(*model2);
    OffscreenRender offscreenRender("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    offscreenRender.setOutputSize(1024, 768, 1, false);
    auto modelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEFAULT_MODE);
    modelSetDisplayer->setupShader("testVS", "testFS", std::unique_ptr<ShaderConstants>(nullptr));
    modelSetDisplayer->initialize(offscreenRender);

    //Display with common displayer for model 2 and 3
    modelSetDisplayer->replaceAllModels(std::vector{model1.get(), model2.get(), model3.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model3->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] != model2->getModelInstanceDisplayers()[0]);
    AssertHelper::assertTrue(model2->getModelInstanceDisplayers()[0] == model3->getModelInstanceDisplayers()[0]);

    //Display with all individual displayers
    model3->updateUv(0, {Point2(99.0f, 99.0f), Point2(99.0f, 99.0f), Point2(99.0f, 99.0f)});
    modelSetDisplayer->replaceAllModels(std::vector{model1.get(), model2.get(), model3.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model3->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] != model2->getModelInstanceDisplayers()[0]);
    AssertHelper::assertTrue(model2->getModelInstanceDisplayers()[0] != model3->getModelInstanceDisplayers()[0]);
}

void ModelSetDisplayerTest::removeInstanceModel() {
    auto model1 = ModelBuilder().newEmptyModel("testModel1");
    auto model2 = std::make_unique<Model>(*model1);
    OffscreenRender offscreenRender("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    offscreenRender.setOutputSize(1024, 768, 1, false);
    auto modelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEFAULT_MODE);
    modelSetDisplayer->setupShader("testVS", "testFS", std::unique_ptr<ShaderConstants>(nullptr));
    modelSetDisplayer->initialize(offscreenRender);

    //Display with common displayer for model 1 and 2
    modelSetDisplayer->replaceAllModels(std::vector{model1.get(), model2.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] == model2->getModelInstanceDisplayers()[0]);
    AssertHelper::assertUnsignedIntEquals(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 1l);
    AssertHelper::assertTrue(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED)[0] == modelSetDisplayer.get());
    AssertHelper::assertUnsignedIntEquals(model2->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 1l);
    AssertHelper::assertTrue(model2->getObservers(Model::NotificationType::MESH_UV_UPDATED)[0] == modelSetDisplayer.get());

    //Remove model 1
    modelSetDisplayer->removeModel(model1.get());
    AssertHelper::assertUnsignedIntEquals(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 0l);

    //Update model 2 and remove
    model2->updateUv(0, {Point2(99.0f, 99.0f), Point2(99.0f, 99.0f), Point2(99.0f, 99.0f)});
    modelSetDisplayer->removeModel(model2.get());
    AssertHelper::assertUnsignedIntEquals(model2->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 0l);
}

void ModelSetDisplayerTest::purgeUnusedDisplayer() {
    auto model1 = ModelBuilder().newEmptyModel("testModel1");
    auto model2 = ModelBuilder().newEmptyModel("testModel2");
    OffscreenRender offscreenRender("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    offscreenRender.setOutputSize(1024, 768, 1, false);
    auto modelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEFAULT_MODE);
    modelSetDisplayer->setupShader("testVS", "testFS", std::unique_ptr<ShaderConstants>(nullptr));
    modelSetDisplayer->initialize(offscreenRender);

    //Display model 1 and 2
    modelSetDisplayer->replaceAllModels(std::vector{model1.get(), model2.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 1l);
    AssertHelper::assertTrue(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED)[0] == modelSetDisplayer.get());
    AssertHelper::assertUnsignedIntEquals(model2->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 1l);
    AssertHelper::assertTrue(model2->getObservers(Model::NotificationType::MESH_UV_UPDATED)[0] == modelSetDisplayer.get());

    //Display only model 2 five minutes later (model 1 displayer is removed)
    model1->getModelInstanceDisplayers()[0]->alterLastRenderingTime(std::chrono::steady_clock::now() - std::chrono::minutes(5));
    model2->getModelInstanceDisplayers()[0]->alterLastRenderingTime(std::chrono::steady_clock::now() - std::chrono::minutes(5));
    modelSetDisplayer->replaceAllModels(std::vector{model2.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 0l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);

    //Destroy model set displayer
    modelSetDisplayer.reset(nullptr);
    AssertHelper::assertUnsignedIntEquals(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 0l); //TODO fix it
    AssertHelper::assertUnsignedIntEquals(model2->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 0l);
}

//TODO add more tests (exclusive)

CppUnit::Test* ModelSetDisplayerTest::suite() {
    auto* suite = new CppUnit::TestSuite("ModelSetDisplayerTest");

    suite->addTest(new CppUnit::TestCaller("updateInstancedModel", &ModelSetDisplayerTest::updateInstancedModel));
    suite->addTest(new CppUnit::TestCaller("removeInstanceModel", &ModelSetDisplayerTest::removeInstanceModel));
    suite->addTest(new CppUnit::TestCaller("purgeUnusedDisplayer", &ModelSetDisplayerTest::purgeUnusedDisplayer));

    return suite;
}
