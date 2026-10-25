#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "3d/scene/ui/widget/button/ButtonTest.h"
#include "3d/scene/ui/VirtualKeyboardFake.h"
#include "AssertHelper.h"
using namespace urchin;

void ButtonTest::focusSwitchBetweenAdjacentButtons() {
    auto uiRenderer = setupUiRenderer();
    auto button1 = Button::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(20.0f, 20.0f, PIXEL), "test", "");
    uiRenderer->addWidget(button1);
    auto button2 = Button::create(nullptr, Position(20.0f, 0.0f, PIXEL), Size(20.0f, 20.0f, PIXEL), "test", "");
    uiRenderer->addWidget(button2);

    uiRenderer->onMouseMove(25.0, 5.0, 0.0, 0.0); //mouse over button 2
    AssertHelper::assertTrue(button2->getWidgetState() == Widget::FOCUS);

    uiRenderer->onMouseMove(5.0, 5.0, 0.0, 0.0); //mouse directly from button 2 to button 1
    AssertHelper::assertTrue(button1->getWidgetState() == Widget::FOCUS);
    AssertHelper::assertTrue(button2->getWidgetState() == Widget::DEFAULT);
}

void ButtonTest::clickOnButtonDeactivateTextBox() {
    auto uiRenderer = setupUiRenderer();
    bool keyboardVisible = false;
    uiRenderer->setupVirtualKeyboard(std::make_unique<VirtualKeyboardFake>(keyboardVisible));
    auto textBox = TextBox::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(40.0f, 20.0f, PIXEL), "test");
    uiRenderer->addWidget(textBox);
    auto button = Button::create(nullptr, Position(0.0f, 30.0f, PIXEL), Size(40.0f, 20.0f, PIXEL), "test", "");
    uiRenderer->addWidget(button);

    uiRenderer->onMouseMove(5.0, 5.0, 0.0, 0.0); //mouse over text box
    uiRenderer->onKeyPress(InputKey::LMB);
    uiRenderer->onKeyRelease(InputKey::LMB);
    AssertHelper::assertTrue(textBox->getWidgetState() == Widget::FOCUS);
    AssertHelper::assertTrue(keyboardVisible);

    uiRenderer->onMouseMove(5.0, 35.0, 0.0, 0.0); //mouse over button
    uiRenderer->onKeyPress(InputKey::LMB);
    AssertHelper::assertTrue(textBox->getWidgetState() == Widget::DEFAULT);
    AssertHelper::assertTrue(button->getWidgetState() == Widget::CLICKING);
    AssertHelper::assertFalse(keyboardVisible);
}

void ButtonTest::validateKeyConsumedOnNavigationFocus() {
    auto uiRenderer = setupUiRenderer();
    auto button = Button::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(20.0f, 20.0f, PIXEL), "test", "");
    uiRenderer->addWidget(button);

    button->updateGameControllerFocus(true);
    AssertHelper::assertFalse(uiRenderer->onKeyPress(InputKey::ENTER));
    AssertHelper::assertTrue(button->getWidgetState() == Widget::CLICKING);
    AssertHelper::assertFalse(uiRenderer->onKeyRelease(InputKey::ENTER));
    AssertHelper::assertTrue(button->getWidgetState() == Widget::FOCUS);
}

std::unique_ptr<UIRenderer> ButtonTest::setupUiRenderer() {
    renderTarget = std::make_unique<OffscreenRender>("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    renderTarget->setOutputSize(1920, 1080, 1, false);
    i18nService = std::make_unique<I18nService>();
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    return std::make_unique<UIRenderer>(1.0f, *renderTarget, *i18nService);
}

CppUnit::Test* ButtonTest::suite() {
    auto* suite = new CppUnit::TestSuite("ButtonTest");

    suite->addTest(new CppUnit::TestCaller("focusSwitchBetweenAdjacentButtons", &ButtonTest::focusSwitchBetweenAdjacentButtons));
    suite->addTest(new CppUnit::TestCaller("clickOnButtonDeactivateTextBox", &ButtonTest::clickOnButtonDeactivateTextBox));
    suite->addTest(new CppUnit::TestCaller("validateKeyConsumedOnNavigationFocus", &ButtonTest::validateKeyConsumedOnNavigationFocus));

    return suite;
}
