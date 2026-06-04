#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <Urchin3dEngine.h>

#include "3d/scene/renderer3d/model/displayer/ModelSetDisplayerTest.h"
#include "AssertHelper.h"
using namespace urchin;

void ModelSetDisplayerTest::test() { //TODO renaming
    OffscreenRender offscreenRender("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    offscreenRender.setOutputSize(1024, 712, 1, false);
    auto model1 = ModelBuilder().newEmptyModel("testModel1");
    auto model2 = ModelBuilder().newEmptyModel("testModel2");
    auto model3 = std::make_unique<Model>(*model2);
    auto modelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEFAULT_MODE);
    modelSetDisplayer->setupShader("testVS", "testFS", std::unique_ptr<ShaderConstants>(nullptr));
    modelSetDisplayer->initialize(offscreenRender);
    modelSetDisplayer->addNewModel(model1.get());
    modelSetDisplayer->addNewModel(model2.get());
    modelSetDisplayer->addNewModel(model3.get());

    AssertHelper::assertTrue(model1->getModelInstanceDisplayers()[0] != model2->getModelInstanceDisplayers()[0]);
    AssertHelper::assertTrue(model2->getModelInstanceDisplayers()[0] == model3->getModelInstanceDisplayers()[0]);
}

CppUnit::Test* ModelSetDisplayerTest::suite() {
    auto* suite = new CppUnit::TestSuite("ModelSetDisplayerTest");

    suite->addTest(new CppUnit::TestCaller("test", &ModelSetDisplayerTest::test));

    return suite;
}
