#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "3d/scene/ui/widget/textbox/TextBoxTest.h"
#include "AssertHelper.h"
using namespace urchin;

void TextBoxTest::textShift() {
    auto uiRenderer = setupUiRenderer();
    auto textBox = TextBox::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(32.0f, 20.0f, PIXEL), "test"); //max width text: 32px - margin 2px - cursor 2px = 28px
    uiRenderer->addWidget(textBox);

    std::string textValue = "abcdefg"; //text box can only display 'abcde' (a: 5px, b: 6px, c: 5px, d: 6px, e: 5px, f: 3px, g: 6px)
    uiRenderer->onMouseMove(1.0, 1.0, 0.0, 0.0); //move mouse over text box
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT); //activate text box
    for (char textLetter : textValue) {
        uiRenderer->onChar(static_cast<char32_t>(textLetter));
    }
    AssertHelper::assertStringEquals(textBox->getTextWidget().getBaseText(), "cdefg");

    for (std::size_t i = 0; i < textValue.size(); ++i) {
        uiRenderer->onKeyPress(InputDeviceKey::LEFT_ARROW);
    }
    AssertHelper::assertStringEquals(textBox->getTextWidget().getBaseText(), "abcde");
}

void TextBoxTest::textSelection() {
    auto uiRenderer = setupUiRenderer();
    auto textBox = TextBox::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(40.0f, 20.0f, PIXEL), "test"); //max width text: 40px - margin 2px - cursor 2px = 36px
    uiRenderer->addWidget(textBox);

    std::string textValue = "0123456789"; //text box can only display the text partially (each number: 5px)
    uiRenderer->onMouseMove(1.0, 1.0, 0.0, 0.0); //move mouse over text box
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT); //activate text box
    for (char textLetter : textValue) {
        uiRenderer->onChar(static_cast<char32_t>(textLetter));
    }

    for (std::size_t i = 0; i < textValue.size(); ++i) {
        uiRenderer->onKeyPress(InputDeviceKey::LEFT_ARROW);
    }
    AssertHelper::assertStringEquals(textBox->getTextWidget().getBaseText(), "0123456");

    uiRenderer->onKeyPress(InputDeviceKey::CTRL);
    uiRenderer->onKeyPress(InputDeviceKey::A); //select all
    uiRenderer->onKeyRelease(InputDeviceKey::CTRL);
    AssertHelper::assertStringEquals(textBox->getTextWidget().getBaseText(), "3456789");

    uiRenderer->onKeyPress(InputDeviceKey::DELETE_KEY); //delete selection
    AssertHelper::assertStringEquals(textBox->getTextWidget().getBaseText(), "");
}

std::unique_ptr<UIRenderer> TextBoxTest::setupUiRenderer() {
    renderTarget = std::make_unique<OffscreenRender>("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    renderTarget->setOutputSize(1920, 1080, 1, false);
    i18nService = std::make_unique<I18nService>();
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    return std::make_unique<UIRenderer>(1.0f, *renderTarget, *i18nService);
}

CppUnit::Test* TextBoxTest::suite() {
    auto* suite = new CppUnit::TestSuite("TextBoxTest");

    suite->addTest(new CppUnit::TestCaller("textShift", &TextBoxTest::textShift));
    suite->addTest(new CppUnit::TestCaller("textSelection", &TextBoxTest::textSelection));

    return suite;
}