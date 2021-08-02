#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/ui/UIRendererTest.h>
#include <3d/scene/ui/DetachChildrenEventListener.h>
#include <AssertHelper.h>
using namespace urchin;

void UIRendererTest::focusState() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, LengthType::PIXEL), Size(100.0f, 100.0f, LengthType::PIXEL), "test");
    uiRenderer->addWidget(container);
    auto widget = StaticBitmap::create(container.get(), Position(20.0f, 20.0f, LengthType::PIXEL), Size(60.0f, 60.0f, LengthType::PIXEL), "ui/widget/empty.tga");

    uiRenderer->onMouseMove(50.0, 50.0);

    AssertHelper::assertIntEquals(container->getWidgetState(), Widget::WidgetStates::FOCUS);
}

void UIRendererTest::noFocusStateBecauseOutsideContainer() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, LengthType::PIXEL), Size(100.0f, 10.0f, LengthType::PIXEL), "test");
    uiRenderer->addWidget(container);
    auto widget = StaticBitmap::create(container.get(), Position(20.0f, 20.0f, LengthType::PIXEL), Size(60.0f, 60.0f, LengthType::PIXEL), "ui/widget/empty.tga");

    uiRenderer->onMouseMove(50.0, 50.0);

    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetStates::DEFAULT);
}

void UIRendererTest::clickingState() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, LengthType::PIXEL), Size(60.0f, 60.0f, LengthType::PIXEL), "ui/widget/empty.tga");
    uiRenderer->addWidget(widget);

    uiRenderer->onMouseMove(50.0, 50.0);
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetStates::CLICKING);

    uiRenderer->onKeyRelease(InputDeviceKey::MOUSE_LEFT);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetStates::FOCUS);
}

void UIRendererTest::noClickingStateBecauseMouseOutside() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, LengthType::PIXEL), Size(60.0f, 60.0f, LengthType::PIXEL), "ui/widget/empty.tga");
    uiRenderer->addWidget(widget);

    uiRenderer->onMouseMove(10.0, 10.0);
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetStates::DEFAULT);
}

void UIRendererTest::parentPixelPosition() {
    auto uiRenderer = setupUiRenderer();
    auto parentWidget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, LengthType::PIXEL), Size(60.0f, 60.0f, LengthType::PIXEL), "ui/widget/empty.tga");
    uiRenderer->addWidget(parentWidget);
    auto widget = StaticBitmap::create(parentWidget.get(), Position(10.0f, 10.0f, LengthType::PIXEL), Size(5.0f, 5.0f, LengthType::PIXEL), "ui/widget/empty.tga");

    AssertHelper::assertFloatEquals(widget->getGlobalPositionX(), 20.0f + 10.0f);
    AssertHelper::assertFloatEquals(widget->getGlobalPositionY(), 20.0f + 10.0f);
}

void UIRendererTest::relativeParentPixelPosition() {
    auto uiRenderer = setupUiRenderer();
    auto parentWidget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, LengthType::PIXEL), Size(60.0f, 60.0f, LengthType::PIXEL), "ui/widget/empty.tga");
    uiRenderer->addWidget(parentWidget);
    auto widget = StaticBitmap::create(parentWidget.get(), Position(10.0f, 10.0f, LengthType::PIXEL, RelativeTo::PARENT_TOP_RIGHT), Size(5.0f, 5.0f, LengthType::PIXEL), "ui/widget/empty.tga");

    AssertHelper::assertFloatEquals(widget->getGlobalPositionX(), 20.0f + 60.0f + 10.0f);
    AssertHelper::assertFloatEquals(widget->getGlobalPositionY(), 20.0f + 10.0f);
}

void UIRendererTest::screenPercentagePosition() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, LengthType::SCREEN_PERCENT), Size(60.0f, 60.0f, LengthType::PIXEL), "ui/widget/empty.tga");
    uiRenderer->addWidget(widget);

    AssertHelper::assertFloatEquals(widget->getGlobalPositionX(), 1920.0f * 0.2f);
    AssertHelper::assertFloatEquals(widget->getGlobalPositionY(), 1080.0f * 0.2f);
}

void UIRendererTest::containerPercentagePosition() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, LengthType::PIXEL), Size(100.0f, 100.0f, LengthType::PIXEL), "test");
    uiRenderer->addWidget(container);
    auto widget = StaticBitmap::create(container.get(), Position(20.0f, 20.0f, LengthType::CONTAINER_PERCENT), Size(60.0f, 60.0f, LengthType::PIXEL), "ui/widget/empty.tga");

    AssertHelper::assertFloatEquals(widget->getGlobalPositionX(), 100.0f * 0.2f);
    AssertHelper::assertFloatEquals(widget->getGlobalPositionY(), 100.0f * 0.2f);
}

void UIRendererTest::relativeLengthSize() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(0.0f, 0.0f, LengthType::PIXEL), Size(10.0f, LengthType::PIXEL, 2.0f, LengthType::RELATIVE_LENGTH), "ui/widget/empty.tga");
    uiRenderer->addWidget(widget);

    AssertHelper::assertUnsignedIntEquals(widget->getWidth(), 10);
    AssertHelper::assertUnsignedIntEquals(widget->getHeight(), 10 * 2);
}

void UIRendererTest::buttonRemoveParentContainer() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, LengthType::PIXEL), Size(100.0f, 100.0f, LengthType::PIXEL), "test");
    uiRenderer->addWidget(container);
    std::weak_ptr<Container> childContainer = Container::create(container.get(), Position(0.0f, 0.0f, LengthType::PIXEL), Size(100.0f, 100.0f, LengthType::PIXEL), "test");
    std::weak_ptr<StaticBitmap> deleteButton = StaticBitmap::create(childContainer.lock().get(), Position(0.0f, 0.0f, LengthType::PIXEL), Size(100.0f, 100.0f, LengthType::PIXEL), "ui/widget/empty.tga");
    deleteButton.lock()->addEventListener(std::make_unique<DetachChildrenEventListener>(container.get()));

    uiRenderer->onMouseMove(50.0, 50.0);
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT);

    AssertHelper::assertTrue(childContainer.expired());
    AssertHelper::assertTrue(deleteButton.expired());
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

    //widget events
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("focusState", &UIRendererTest::focusState));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("noFocusStateBecauseOutsideContainer", &UIRendererTest::noFocusStateBecauseOutsideContainer));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("clickingState", &UIRendererTest::clickingState));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("noClickingStateBecauseMouseOutside", &UIRendererTest::noClickingStateBecauseMouseOutside));

    //positioning
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("parentPixelPosition", &UIRendererTest::parentPixelPosition));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("relativeParentPosition", &UIRendererTest::relativeParentPixelPosition));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("screenPercentagePosition", &UIRendererTest::screenPercentagePosition));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("containerPercentagePosition", &UIRendererTest::containerPercentagePosition));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("relativeLengthSize", &UIRendererTest::relativeLengthSize));

    //event listener
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("buttonRemoveParentContainer", &UIRendererTest::buttonRemoveParentContainer));

    return suite;
}