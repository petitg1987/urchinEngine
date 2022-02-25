#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/ui/UIRendererTest.h>
#include <3d/scene/ui/DetachChildrenEventListener.h>
#include <3d/scene/ui/FocusEventListener.h>
#include <AssertHelper.h>
using namespace urchin;

void UIRendererTest::focusState() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL));
    uiRenderer->addWidget(container);
    auto widget = StaticBitmap::create(container.get(), Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");

    uiRenderer->onMouseMove(50.0, 50.0);

    AssertHelper::assertIntEquals(container->getWidgetState(), Widget::WidgetStates::FOCUS);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetStates::FOCUS);
}

void UIRendererTest::noFocusStateBecauseOutsideContainer() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 10.0f, PIXEL));
    uiRenderer->addWidget(container);
    auto widget = StaticBitmap::create(container.get(), Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");

    uiRenderer->onMouseMove(50.0, 50.0);

    AssertHelper::assertIntEquals(container->getWidgetState(), Widget::WidgetStates::DEFAULT);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetStates::DEFAULT);
}

void UIRendererTest::clickingState() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");
    uiRenderer->addWidget(widget);

    uiRenderer->onMouseMove(50.0, 50.0);
    uiRenderer->onKeyPress((unsigned int)InputDeviceKey::MOUSE_LEFT);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetStates::CLICKING);

    uiRenderer->onKeyRelease((unsigned int)InputDeviceKey::MOUSE_LEFT);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetStates::FOCUS);
}

void UIRendererTest::noClickingStateBecauseMouseOutside() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");
    uiRenderer->addWidget(widget);

    uiRenderer->onMouseMove(10.0, 10.0);
    uiRenderer->onKeyPress((unsigned int)InputDeviceKey::MOUSE_LEFT);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetStates::DEFAULT);
}

void UIRendererTest::focusLostEvent() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL), "ui/widget/empty.png");
    bool focused = false;
    bool focusLost = false;
    widget->addEventListener(std::make_unique<FocusEventListener>(focused, focusLost));
    uiRenderer->addWidget(widget);
    uiRenderer->onMouseMove(50.0, 50.0);
    assert(focused);

    uiRenderer->onMouseMove(110.0, 50.0);

    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetStates::DEFAULT);
    AssertHelper::assertTrue(focusLost);
}

void UIRendererTest::focusLostEventWithClick() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL), "ui/widget/empty.png");
    bool focused = false;
    bool focusLost = false;
    widget->addEventListener(std::make_unique<FocusEventListener>(focused, focusLost));
    uiRenderer->addWidget(widget);
    uiRenderer->onMouseMove(50.0, 50.0);
    assert(focused);

    uiRenderer->onKeyPress((unsigned int)InputDeviceKey::MOUSE_LEFT);
    uiRenderer->onMouseMove(110.0, 50.0);
    uiRenderer->onKeyRelease((unsigned int)InputDeviceKey::MOUSE_LEFT);

    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetStates::DEFAULT);
    AssertHelper::assertTrue(focusLost);
}

void UIRendererTest::parentPixelPosition() {
    auto uiRenderer = setupUiRenderer();
    auto parentWidget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");
    uiRenderer->addWidget(parentWidget);
    auto widget = StaticBitmap::create(parentWidget.get(), Position(10.0f, 10.0f, PIXEL), Size(5.0f, 5.0f, PIXEL), "ui/widget/empty.png");

    AssertHelper::assertIntEquals(widget->getGlobalPositionX(), 20 + 10);
    AssertHelper::assertIntEquals(widget->getGlobalPositionY(), 20 + 10);
}

void UIRendererTest::relativeParentPixelPosition() {
    auto uiRenderer = setupUiRenderer();
    auto parentWidget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");
    uiRenderer->addWidget(parentWidget);
    auto widget = StaticBitmap::create(parentWidget.get(), Position(10.0f, 10.0f, PIXEL, RelativeTo::PARENT_RIGHT_TOP), Size(5.0f, 5.0f, PIXEL), "ui/widget/empty.png");

    AssertHelper::assertIntEquals(widget->getGlobalPositionX(), 20 + 60 + 10);
    AssertHelper::assertIntEquals(widget->getGlobalPositionY(), 20 + 10);
}

void UIRendererTest::screenPercentagePosition() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, SCREEN_PERCENT), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");
    uiRenderer->addWidget(widget);

    AssertHelper::assertIntEquals(widget->getGlobalPositionX(), (int)(1920.0f * 0.2f));
    AssertHelper::assertIntEquals(widget->getGlobalPositionY(), (int)(1080.0f * 0.2f));
}

void UIRendererTest::containerPercentagePosition() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL));
    uiRenderer->addWidget(container);
    auto widget = StaticBitmap::create(container.get(), Position(20.0f, 20.0f, CONTAINER_PERCENT), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");

    AssertHelper::assertIntEquals(widget->getGlobalPositionX(), (int)(100.0f * 0.2f));
    AssertHelper::assertIntEquals(widget->getGlobalPositionY(), (int)(100.0f * 0.2f));
}

void UIRendererTest::relativeLengthSize() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(10.0f, PIXEL, 2.0f, RATIO_TO_WIDTH), "ui/widget/empty.png");
    uiRenderer->addWidget(widget);

    AssertHelper::assertUnsignedIntEquals(widget->getWidth(), 10);
    AssertHelper::assertUnsignedIntEquals(widget->getHeight(), 10 * 2);
}

void UIRendererTest::buttonRemoveParentContainer() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL));
    uiRenderer->addWidget(container);
    std::weak_ptr<Container> childContainer = Container::create(container.get(), Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL));
    std::weak_ptr<StaticBitmap> deleteButton = StaticBitmap::create(childContainer.lock().get(), Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL), "ui/widget/empty.png");
    deleteButton.lock()->addEventListener(std::make_unique<DetachChildrenEventListener>(container.get()));

    uiRenderer->onMouseMove(50.0, 50.0);
    uiRenderer->onKeyPress((unsigned int)InputDeviceKey::MOUSE_LEFT);

    AssertHelper::assertTrue(childContainer.expired());
    AssertHelper::assertTrue(deleteButton.expired());
}

void UIRendererTest::containerWithLazyWidgets() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::createScrollable(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL), "test");
    uiRenderer->addWidget(container);
    auto loadChildrenFunction = [](LazyWidget* parent) {
        StaticBitmap::create(parent, Position(0.0f, 0.0f, PIXEL), Size(1.0f, 1.0f, PIXEL), "ui/widget/empty.png");
    };
    auto lazyWidget1 = LazyWidget::create(container.get(), Position(1.0f, 1.0f, PIXEL), Size(1.0f, 50.0f, PIXEL), loadChildrenFunction);
    auto lazyWidget2 = LazyWidget::create(container.get(), Position(1.0f, 101.0f, PIXEL), Size(1.0f, 50.0f, PIXEL), loadChildrenFunction);
    StaticBitmap::create(container.get(), Position(0.0f, 1000.0f, PIXEL), Size(1.0f, 1.0f, PIXEL), "ui/widget/empty.png");
    uiRenderer->onMouseMove(50.0, 50.0);

    AssertHelper::assertUnsignedIntEquals(lazyWidget1->getChildren().size(), 1);
    AssertHelper::assertUnsignedIntEquals(lazyWidget2->getChildren().size(), 0);

    uiRenderer->onScroll(-1.0);

    AssertHelper::assertUnsignedIntEquals(lazyWidget1->getChildren().size(), 1);
    AssertHelper::assertUnsignedIntEquals(lazyWidget2->getChildren().size(), 1);

    uiRenderer->onScroll(-10.0);

    AssertHelper::assertUnsignedIntEquals(lazyWidget1->getChildren().size(), 0);
    AssertHelper::assertUnsignedIntEquals(lazyWidget2->getChildren().size(), 1);

    uiRenderer->onScroll(-20.0);

    AssertHelper::assertUnsignedIntEquals(lazyWidget1->getChildren().size(), 0);
    AssertHelper::assertUnsignedIntEquals(lazyWidget2->getChildren().size(), 0);
}

std::unique_ptr<UIRenderer> UIRendererTest::setupUiRenderer() {
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
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("focusLostEvent", &UIRendererTest::focusLostEvent));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("focusLostEventWithClick", &UIRendererTest::focusLostEventWithClick));

    //positioning
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("parentPixelPosition", &UIRendererTest::parentPixelPosition));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("relativeParentPosition", &UIRendererTest::relativeParentPixelPosition));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("screenPercentagePosition", &UIRendererTest::screenPercentagePosition));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("containerPercentagePosition", &UIRendererTest::containerPercentagePosition));
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("relativeLengthSize", &UIRendererTest::relativeLengthSize));

    //event listener
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("buttonRemoveParentContainer", &UIRendererTest::buttonRemoveParentContainer));

    //lazy loading
    suite->addTest(new CppUnit::TestCaller<UIRendererTest>("containerWithLazyWidgets", &UIRendererTest::containerWithLazyWidgets));

    return suite;
}
