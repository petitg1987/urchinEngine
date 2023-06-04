#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <Urchin3dEngine.h>

#include <3d/graphics/render/GenericRendererComparatorTest.h>
#include <AssertHelper.h>
using namespace urchin;

void GenericRendererComparatorTest::depthTestOrdering() {
    std::vector<GenericRenderer*> renderers;
    NullRenderTarget nullRenderTarget(1920, 1080);
    std::unique_ptr<Shader> shader = ShaderBuilder::createNullShader();
    //rendering order: 1, no depth test
    std::unique_ptr<GenericRenderer> r1 = GenericRendererBuilder::create("r1", nullRenderTarget, *shader, ShapeType::TRIANGLE)->build();
    r1->disableRenderer();
    r1->enableRenderer(1);
    renderers.push_back(r1.get());
    //rendering order: 1, depth test
    std::unique_ptr<GenericRenderer> r2 = GenericRendererBuilder::create("r2", nullRenderTarget, *shader, ShapeType::TRIANGLE)->enableDepthTest()->build();
    r2->disableRenderer();
    r2->enableRenderer(1);
    renderers.push_back(r2.get());
    //rendering order: 0, no depth test
    std::unique_ptr<GenericRenderer> r3 = GenericRendererBuilder::create("r3", nullRenderTarget, *shader, ShapeType::TRIANGLE)->build();
    renderers.push_back(r3.get());
    //rendering order: 0, depth test
    std::unique_ptr<GenericRenderer> r4 = GenericRendererBuilder::create("r4", nullRenderTarget, *shader, ShapeType::TRIANGLE)->enableDepthTest()->build();
    renderers.push_back(r4.get());

    std::ranges::sort(renderers, PipelineProcessorComparator());

    AssertHelper::assertStringEquals(renderers[0]->getName(), "r4"); //rendering order: 0, depth test
    AssertHelper::assertStringEquals(renderers[1]->getName(), "r3"); //rendering order: 0, no depth test
    AssertHelper::assertStringEquals(renderers[2]->getName(), "r2"); //rendering order: 1, depth test
    AssertHelper::assertStringEquals(renderers[3]->getName(), "r1"); //rendering order: 1, no depth test
}

CppUnit::Test* GenericRendererComparatorTest::suite() {
    auto* suite = new CppUnit::TestSuite("GenericRendererComparatorTest");

    suite->addTest(new CppUnit::TestCaller<GenericRendererComparatorTest>("depthTestOrdering", &GenericRendererComparatorTest::depthTestOrdering));

    return suite;
}
