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
    modelSetDisplayer->registerModel(model1.get()); //register a model in advance to improve performance of 'setModelsToDisplay'
    modelSetDisplayer->setModelsToDisplay(std::vector{model1.get(), model2.get(), model3.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model3->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] != model2->getModelInstanceDisplayers()[0]);
    AssertHelper::assertTrue(model2->getModelInstanceDisplayers()[0] == model3->getModelInstanceDisplayers()[0]);
    ModelInstanceDisplayer* initialModel2And3Displayer = model2->getModelInstanceDisplayers()[0];

    //Display with all individual displayers
    model3->setLightMask(0);
    modelSetDisplayer->setModelsToDisplay(std::vector{model1.get(), model2.get(), model3.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model3->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] != model2->getModelInstanceDisplayers()[0]);
    AssertHelper::assertTrue(model2->getModelInstanceDisplayers()[0] != model3->getModelInstanceDisplayers()[0]);

    //Display with common displayer for model 2 and 3
    model3->setLightMask(std::numeric_limits<uint8_t>::max());
    modelSetDisplayer->setModelsToDisplay(std::vector{model1.get(), model2.get(), model3.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model3->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] != model2->getModelInstanceDisplayers()[0]);
    AssertHelper::assertTrue(model2->getModelInstanceDisplayers()[0] == initialModel2And3Displayer);
    AssertHelper::assertTrue(model3->getModelInstanceDisplayers()[0] == initialModel2And3Displayer);
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
    modelSetDisplayer->setModelsToDisplay(std::vector{model1.get(), model2.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] == model2->getModelInstanceDisplayers()[0]);
    AssertHelper::assertUnsignedIntEquals(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 1l);
    AssertHelper::assertTrue(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED)[0] == modelSetDisplayer.get());
    AssertHelper::assertUnsignedIntEquals(model2->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 1l);
    AssertHelper::assertTrue(model2->getObservers(Model::NotificationType::MESH_UV_UPDATED)[0] == modelSetDisplayer.get());

    //Remove model 1
    modelSetDisplayer->unregisterModel(model1.get());
    AssertHelper::assertUnsignedIntEquals(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 0l);

    //Update model 2 and remove
    model2->updateUv(0, {Point2(99.0f, 99.0f), Point2(99.0f, 99.0f), Point2(99.0f, 99.0f)});
    modelSetDisplayer->unregisterModel(model2.get());
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
    modelSetDisplayer->setModelsToDisplay(std::vector{model1.get(), model2.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 1l);
    AssertHelper::assertTrue(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED)[0] == modelSetDisplayer.get());
    AssertHelper::assertUnsignedIntEquals(model2->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 1l);
    AssertHelper::assertTrue(model2->getObservers(Model::NotificationType::MESH_UV_UPDATED)[0] == modelSetDisplayer.get());

    //Display only model 2 five minutes later (model 1 displayer is removed)
    modelSetDisplayer->alterRegisterModelLastUsageTime(model1.get(), std::chrono::steady_clock::now() - std::chrono::minutes(5));
    modelSetDisplayer->alterRegisterModelLastUsageTime(model2.get(), std::chrono::steady_clock::now() - std::chrono::minutes(5));
    modelSetDisplayer->setModelsToDisplay(std::vector{model2.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 0l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);

    //Display model 1 and 2 (model 1 displayed is re-created)
    modelSetDisplayer->setModelsToDisplay(std::vector{model1.get(), model2.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);

    //Display only model 1 five minutes later (model 2 displayer is removed)
    modelSetDisplayer->alterRegisterModelLastUsageTime(model1.get(), std::chrono::steady_clock::now() - std::chrono::minutes(5));
    modelSetDisplayer->alterRegisterModelLastUsageTime(model2.get(), std::chrono::steady_clock::now() - std::chrono::minutes(5));
    modelSetDisplayer->setModelsToDisplay(std::vector{model1.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 0l);

    //Destroy model set displayer
    modelSetDisplayer.reset(nullptr);
    AssertHelper::assertUnsignedIntEquals(model1->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 0l);
    AssertHelper::assertUnsignedIntEquals(model2->getObservers(Model::NotificationType::MESH_UV_UPDATED).size(), 0l);
}

void ModelSetDisplayerTest::modelWithoutInstancing() {
    auto model1 = ModelBuilder().newEmptyModel("testModel1");
    model1->updateUv(0, {Point2(99.0f, 99.0f), Point2(99.0f, 99.0f), Point2(99.0f, 99.0f)}); //cannot be instanced
    auto model2 = std::make_unique<Model>(*model1);
    OffscreenRender offscreenRender("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    offscreenRender.setOutputSize(1024, 768, 1, false);
    auto modelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEFAULT_MODE);
    modelSetDisplayer->setupShader("testVS", "testFS", std::unique_ptr<ShaderConstants>(nullptr));
    modelSetDisplayer->initialize(offscreenRender);

    //Display with all individual displayers
    modelSetDisplayer->setModelsToDisplay(std::vector{model1.get(), model2.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] != model2->getModelInstanceDisplayers()[0]);
    ModelInstanceDisplayer* initialModel1Displayer = model1->getModelInstanceDisplayers()[0];
    ModelInstanceDisplayer* initialModel2Displayer = model2->getModelInstanceDisplayers()[0];

    //Display only model 1
    modelSetDisplayer->setModelsToDisplay(std::vector{model1.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());

    //Display with existing displayer
    modelSetDisplayer->setModelsToDisplay(std::vector{model1.get(), model2.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] == initialModel1Displayer);
    AssertHelper::assertTrue(model2->getModelInstanceDisplayers()[0] == initialModel2Displayer);
}

void ModelSetDisplayerTest::layerMaskRestriction() {
    auto model1 = ModelBuilder().newEmptyModel("testModel1");
    auto model2 = std::make_unique<Model>(*model1);
    OffscreenRender offscreenRender("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    offscreenRender.setOutputSize(1024, 768, 1, false);
    auto modelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEFAULT_MODE);
    modelSetDisplayer->setupShader("testVS", "testFS", std::unique_ptr<ShaderConstants>(nullptr));
    modelSetDisplayer->initialize(offscreenRender);

    //Display with common displayer for model 1 and 2
    modelSetDisplayer->resetModelsToDisplay();
    modelSetDisplayer->addModelToDisplay(model1.get(), std::bitset<8>(1));
    modelSetDisplayer->addModelToDisplay(model2.get(), std::bitset<8>(2));
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] == model2->getModelInstanceDisplayers()[0]);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0]->getLayersMask() == std::bitset<8>(3));

    //Display with all individual displayers
    model1->setLightMask(0);
    modelSetDisplayer->resetModelsToDisplay();
    modelSetDisplayer->addModelToDisplay(model1.get(), std::bitset<8>(1));
    modelSetDisplayer->addModelToDisplay(model2.get(), std::bitset<8>(2));
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0]->getLayersMask() == std::bitset<8>(1));
    AssertHelper::assertTrue(model2->getModelInstanceDisplayers()[0]->getLayersMask() == std::bitset<8>(2));
}

CppUnit::Test* ModelSetDisplayerTest::suite() {
    auto* suite = new CppUnit::TestSuite("ModelSetDisplayerTest");

    suite->addTest(new CppUnit::TestCaller("updateInstancedModel", &ModelSetDisplayerTest::updateInstancedModel));
    suite->addTest(new CppUnit::TestCaller("removeInstanceModel", &ModelSetDisplayerTest::removeInstanceModel));
    suite->addTest(new CppUnit::TestCaller("purgeUnusedDisplayer", &ModelSetDisplayerTest::purgeUnusedDisplayer));
    suite->addTest(new CppUnit::TestCaller("modelWithoutInstancing", &ModelSetDisplayerTest::modelWithoutInstancing));
    suite->addTest(new CppUnit::TestCaller("layerMaskRestriction", &ModelSetDisplayerTest::layerMaskRestriction));

    return suite;
}
