#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/ui/UIRendererTest.h>
#include <3d/scene/ui/DetachChildrenEventListener.h>
#include <AssertHelper.h>
using namespace urchin;

void UIRendererTest::childRemoveItsParent() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, LengthType::PIXEL), Size(100.0f, 100.0f, LengthType::PIXEL), "test");
    uiRenderer->addWidget(container);
    std::weak_ptr<Container> childContainer = Container::create(container.get(), Position(0.0f, 0.0f, LengthType::PIXEL), Size(100.0f, 100.0f, LengthType::PIXEL), "test");
    std::weak_ptr<Container> childChildContainer = Container::create(childContainer.lock().get(), Position(0.0f, 0.0f, LengthType::PIXEL), Size(100.0f, 100.0f, LengthType::PIXEL), "test");
    childChildContainer.lock()->addEventListener(std::make_unique<DetachChildrenEventListener>(container.get()));

    uiRenderer->onMouseMove(50.0, 50.0);
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT);

    AssertHelper::assertTrue(childContainer.expired());
    AssertHelper::assertTrue(childChildContainer.expired());
}

std::unique_ptr<UIRenderer> UIRendererTest::setupUiRenderer() {
    FileSystem::instance().setupResourcesDirectory("resources/");

    renderTarget = std::make_unique<NullRenderTarget>(1920, 1080);
    i18nService = std::make_unique<I18nService>();
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    return std::make_unique<UIRenderer>(*renderTarget, *i18nService);
}

CppUnit::Test* UIRendererTest::suite() {
    auto* suite = new CppUnit::TestSuite("UIRendererTest");

    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("childRemoveItsParent", &UIRendererTest::childRemoveItsParent));

    return suite;
}
