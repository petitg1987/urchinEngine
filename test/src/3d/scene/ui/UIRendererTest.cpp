#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/ui/UIRendererTest.h>
#include <3d/scene/ui/DetachChildrenEventListener.h>
#include <3d/scene/ui/FocusEventListener.h>
#include <scene/ui/displayer/WidgetSetDisplayer.h>
#include <AssertHelper.h>
using namespace urchin;

void UIRendererTest::focusState() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL));
    uiRenderer->addWidget(container);
    auto widget = StaticBitmap::create(container.get(), Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");

    uiRenderer->onMouseMove(50.0, 50.0, 0.0, 0.0);

    AssertHelper::assertIntEquals(container->getWidgetState(), Widget::WidgetState::FOCUS);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetState::FOCUS);
}

void UIRendererTest::noFocusStateBecauseOutsideContainer() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 10.0f, PIXEL));
    uiRenderer->addWidget(container);
    auto widget = StaticBitmap::create(container.get(), Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");

    uiRenderer->onMouseMove(50.0, 50.0, 0.0, 0.0);

    AssertHelper::assertIntEquals(container->getWidgetState(), Widget::WidgetState::DEFAULT);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetState::DEFAULT);
}

void UIRendererTest::clickingState() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");
    uiRenderer->addWidget(widget);

    uiRenderer->onMouseMove(50.0, 50.0, 0.0, 0.0);
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetState::CLICKING);

    uiRenderer->onKeyRelease(InputDeviceKey::MOUSE_LEFT);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetState::FOCUS);
}

void UIRendererTest::noClickingStateBecauseMouseOutside() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");
    uiRenderer->addWidget(widget);

    uiRenderer->onMouseMove(10.0, 10.0, 0.0, 0.0);
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT);
    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetState::DEFAULT);
}

void UIRendererTest::focusLostEvent() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL), "ui/widget/empty.png");
    bool focused = false;
    bool focusLost = false;
    widget->addEventListener(std::make_unique<FocusEventListener>(focused, focusLost));
    uiRenderer->addWidget(widget);
    uiRenderer->onMouseMove(50.0, 50.0, 0.0, 0.0);
    assert(focused);

    uiRenderer->onMouseMove(110.0, 50.0, 0.0, 0.0);

    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetState::DEFAULT);
    AssertHelper::assertTrue(focusLost);
}

void UIRendererTest::focusLostEventWithClick() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL), "ui/widget/empty.png");
    bool focused = false;
    bool focusLost = false;
    widget->addEventListener(std::make_unique<FocusEventListener>(focused, focusLost));
    uiRenderer->addWidget(widget);
    uiRenderer->onMouseMove(50.0, 50.0, 0.0, 0.0);
    assert(focused);

    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT);
    uiRenderer->onMouseMove(110.0, 50.0, 0.0, 0.0);
    uiRenderer->onKeyRelease(InputDeviceKey::MOUSE_LEFT);

    AssertHelper::assertIntEquals(widget->getWidgetState(), Widget::WidgetState::DEFAULT);
    AssertHelper::assertTrue(focusLost);
}

void UIRendererTest::parentPixelPosition() {
    auto uiRenderer = setupUiRenderer();
    auto parentWidget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");
    uiRenderer->addWidget(parentWidget);
    auto widget = StaticBitmap::create(parentWidget.get(), Position(10.0f, 10.0f, PIXEL), Size(5.0f, 5.0f, PIXEL), "ui/widget/empty.png");

    AssertHelper::assertFloatEquals(widget->getGlobalPositionX(), 20.0f + 10.0f);
    AssertHelper::assertFloatEquals(widget->getGlobalPositionY(), 20.0f + 10.0f);
}

void UIRendererTest::relativeParentPixelPosition() {
    auto uiRenderer = setupUiRenderer();
    auto parentWidget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, PIXEL), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");
    uiRenderer->addWidget(parentWidget);
    auto widget = StaticBitmap::create(parentWidget.get(), Position(10.0f, 10.0f, PIXEL, PARENT_RIGHT_TOP), Size(5.0f, 5.0f, PIXEL), "ui/widget/empty.png");

    AssertHelper::assertFloatEquals(widget->getGlobalPositionX(), 20.0f + 60.0f + 10.0f);
    AssertHelper::assertFloatEquals(widget->getGlobalPositionY(), 20.0f + 10.0f);
}

void UIRendererTest::screenPercentagePosition() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(20.0f, 20.0f, SCREEN_PERCENT), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");
    uiRenderer->addWidget(widget);

    AssertHelper::assertFloatEquals(widget->getGlobalPositionX(), 1920.0f * 0.2f);
    AssertHelper::assertFloatEquals(widget->getGlobalPositionY(), 1080.0f * 0.2f);
}

void UIRendererTest::containerPercentagePosition() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL));
    uiRenderer->addWidget(container);
    auto widget = StaticBitmap::create(container.get(), Position(20.0f, 20.0f, CONTAINER_PERCENT), Size(60.0f, 60.0f, PIXEL), "ui/widget/empty.png");

    AssertHelper::assertFloatEquals(widget->getGlobalPositionX(), 100.0f * 0.2f);
    AssertHelper::assertFloatEquals(widget->getGlobalPositionY(), 100.0f * 0.2f);
}

void UIRendererTest::relativeLengthSize() {
    auto uiRenderer = setupUiRenderer();
    auto widget = StaticBitmap::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(10.0f, PIXEL, 2.0f, RATIO_TO_WIDTH), "ui/widget/empty.png");
    uiRenderer->addWidget(widget);

    AssertHelper::assertFloatEquals(widget->getWidth(), 10.0f);
    AssertHelper::assertFloatEquals(widget->getHeight(), 10.0f * 2.0f);
}

void UIRendererTest::buttonRemoveParentContainer() {
    auto uiRenderer = setupUiRenderer();
    auto container = Container::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL));
    uiRenderer->addWidget(container);
    std::weak_ptr childContainer = Container::create(container.get(), Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL));
    std::weak_ptr deleteButton = StaticBitmap::create(childContainer.lock().get(), Position(0.0f, 0.0f, PIXEL), Size(100.0f, 100.0f, PIXEL), "ui/widget/empty.png");
    deleteButton.lock()->addEventListener(std::make_unique<DetachChildrenEventListener>(container.get()));
    std::weak_ptr text = Text::create(deleteButton.lock().get(), Position(0.0f, 0.0f, PIXEL), "test", i18n("my.text"));
    Text* textPTr = text.lock().get();
    AssertHelper::assertTrue(uiRenderer->getI18nService().isTranslatableLabelExist(textPTr));

    uiRenderer->onMouseMove(50.0, 50.0, 0.0, 0.0);
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT);

    AssertHelper::assertTrue(childContainer.expired());
    AssertHelper::assertTrue(deleteButton.expired());
    AssertHelper::assertTrue(text.expired());
    AssertHelper::assertTrue(!uiRenderer->getI18nService().isTranslatableLabelExist(textPTr));
}

void UIRendererTest::removeUIRendererBeforeWidget() {
    std::unique_ptr<UIRenderer> uiRenderer = setupUiRenderer();
    I18nService& i18nService = uiRenderer->getI18nService();
    std::shared_ptr<Text> text = Text::create(nullptr, Position(0.0f, 0.0f, PIXEL), "test", i18n("my.text"));
    uiRenderer->addWidget(text);
    AssertHelper::assertTrue(text->isInitialized());
    AssertHelper::assertTrue(i18nService.isTranslatableLabelExist(text.get()));

    uiRenderer.reset(nullptr);

    AssertHelper::assertTrue(!text->isInitialized());
    AssertHelper::assertTrue(!i18nService.isTranslatableLabelExist(text.get()));
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
    uiRenderer->onMouseMove(50.0, 50.0, 0.0, 0.0);

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

void UIRendererTest::windowChildRenderingOrder() {
    std::unique_ptr<UIRenderer> uiRenderer = setupUiRenderer();

    auto win1 = Window::create(nullptr, Position(10.0f, 10.0f, PIXEL), Size(10.0f, 10.0f, PIXEL), "test", "my.text");
    uiRenderer->addWidget(win1);
    auto widgetWin1Lvl1 = StaticBitmap::create(win1.get(), Position(10.0f, 10.0f, PIXEL, PARENT_RIGHT_TOP), Size(5.0f, 5.0f, PIXEL), "ui/widget/empty.png");
    auto widgetWin1Lvl2 = StaticBitmap::create(widgetWin1Lvl1.get(), Position(10.0f, 10.0f, PIXEL, PARENT_RIGHT_TOP), Size(5.0f, 5.0f, PIXEL), "ui/widget/empty.png");
    auto widgetWin1Lvl1Bis = StaticBitmap::create(win1.get(), Position(10.0f, 10.0f, PIXEL, PARENT_RIGHT_TOP), Size(5.0f, 5.0f, PIXEL), "ui/widget/empty.png");

    auto win2 = Window::create(nullptr, Position(10.0f, 10.0f, PIXEL), Size(10.0f, 10.0f, PIXEL), "test", "my.text");
    uiRenderer->addWidget(win2);
    auto widgetWin2Lvl1 = StaticBitmap::create(win2.get(), Position(10.0f, 10.0f, PIXEL, PARENT_RIGHT_TOP), Size(5.0f, 5.0f, PIXEL), "ui/widget/empty.png");

    unsigned int renderingOrder = 0;
    uiRenderer->prepareRendering(1.0f / 60.0f, renderingOrder, Matrix4<float>());

    std::span<Widget* const> sortedWidgetsToRender = uiRenderer->getWidgetSetDisplayer().getWidgets();
    AssertHelper::assertUnsignedIntEquals(sortedWidgetsToRender.size(), 8);
    AssertHelper::assertTrue(sortedWidgetsToRender[0] == win1.get());
    AssertHelper::assertTrue(sortedWidgetsToRender[2] == widgetWin1Lvl1.get());
    AssertHelper::assertTrue(sortedWidgetsToRender[3] == widgetWin1Lvl1Bis.get());
    AssertHelper::assertTrue(sortedWidgetsToRender[4] == widgetWin1Lvl2.get());
    AssertHelper::assertTrue(sortedWidgetsToRender[5] == win2.get());
    AssertHelper::assertTrue(sortedWidgetsToRender[7] == widgetWin2Lvl1.get());
}

std::unique_ptr<UIRenderer> UIRendererTest::setupUiRenderer() {
    renderTarget = std::make_unique<OffscreenRender>("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    renderTarget->setOutputSize(1920, 1080, 1);
    i18nService = std::make_unique<I18nService>();
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    return std::make_unique<UIRenderer>(1.0f, *renderTarget, *i18nService);
}

CppUnit::Test* UIRendererTest::suite() {
    auto* suite = new CppUnit::TestSuite("UIRendererTest");

    //widget events
    suite->addTest(new CppUnit::TestCaller("focusState", &UIRendererTest::focusState));
    suite->addTest(new CppUnit::TestCaller("noFocusStateBecauseOutsideContainer", &UIRendererTest::noFocusStateBecauseOutsideContainer));
    suite->addTest(new CppUnit::TestCaller("clickingState", &UIRendererTest::clickingState));
    suite->addTest(new CppUnit::TestCaller("noClickingStateBecauseMouseOutside", &UIRendererTest::noClickingStateBecauseMouseOutside));
    suite->addTest(new CppUnit::TestCaller("focusLostEvent", &UIRendererTest::focusLostEvent));
    suite->addTest(new CppUnit::TestCaller("focusLostEventWithClick", &UIRendererTest::focusLostEventWithClick));

    //positioning
    suite->addTest(new CppUnit::TestCaller("parentPixelPosition", &UIRendererTest::parentPixelPosition));
    suite->addTest(new CppUnit::TestCaller("relativeParentPosition", &UIRendererTest::relativeParentPixelPosition));
    suite->addTest(new CppUnit::TestCaller("screenPercentagePosition", &UIRendererTest::screenPercentagePosition));
    suite->addTest(new CppUnit::TestCaller("containerPercentagePosition", &UIRendererTest::containerPercentagePosition));
    suite->addTest(new CppUnit::TestCaller("relativeLengthSize", &UIRendererTest::relativeLengthSize));

    //remove tests
    suite->addTest(new CppUnit::TestCaller("buttonRemoveParentContainer", &UIRendererTest::buttonRemoveParentContainer));
    suite->addTest(new CppUnit::TestCaller("removeUIRendererBeforeWidget", &UIRendererTest::removeUIRendererBeforeWidget));

    //lazy loading
    suite->addTest(new CppUnit::TestCaller("containerWithLazyWidgets", &UIRendererTest::containerWithLazyWidgets));

    //rendering
    suite->addTest(new CppUnit::TestCaller("windowChildRenderingOrder", &UIRendererTest::windowChildRenderingOrder));

    return suite;
}
