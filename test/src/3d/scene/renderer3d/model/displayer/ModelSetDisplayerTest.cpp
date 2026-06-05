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

    modelSetDisplayer->replaceAllModels(std::vector{model1.get(), model2.get(), model3.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model3->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] != model2->getModelInstanceDisplayers()[0]);
    AssertHelper::assertTrue(model2->getModelInstanceDisplayers()[0] == model3->getModelInstanceDisplayers()[0]);

    model3->updateUv(0, {Point2(99.0f, 99.0f), Point2(99.0f, 99.0f), Point2(99.0f, 99.0f)});
    modelSetDisplayer->replaceAllModels(std::vector{model1.get(), model2.get(), model3.get()});
    modelSetDisplayer->prepareRendering(0, Matrix4<float>());
    AssertHelper::assertUnsignedIntEquals(model1->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model2->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertUnsignedIntEquals(model3->getModelInstanceDisplayers().size(), 1l);
    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] != model2->getModelInstanceDisplayers()[0]);
    AssertHelper::assertTrue(model2->getModelInstanceDisplayers()[0] != model3->getModelInstanceDisplayers()[0]);
}

//TODO add more test

CppUnit::Test* ModelSetDisplayerTest::suite() {
    auto* suite = new CppUnit::TestSuite("ModelSetDisplayerTest");

    suite->addTest(new CppUnit::TestCaller("updateInstancedModel", &ModelSetDisplayerTest::updateInstancedModel));

    return suite;
}
