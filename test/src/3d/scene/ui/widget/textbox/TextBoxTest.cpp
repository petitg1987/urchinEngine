#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/ui/widget/textbox/TextBoxTest.h>
#include <AssertHelper.h>
using namespace urchin;

void TextBoxTest::textShift() {
    auto uiRenderer = setupUiRenderer();
    auto textBox = TextBox::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(50.0f, 20.0f, PIXEL), "test");
    uiRenderer->addWidget(textBox);

    std::string textValue = "abcdefg"; //text box can only display 'abcdef'
    uiRenderer->onMouseMove(1.0, 1.0, 0.0, 0.0); //move mouse over text box
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT); //activate text box
    for (char textLetter : textValue) {
        uiRenderer->onChar(static_cast<char32_t>(textLetter));
    }
    AssertHelper::assertStringEquals(textBox->getTextWidget().getBaseText(), "cdefg");

    for (std::size_t i = 0; i < textValue.size(); ++i) {
        uiRenderer->onKeyPress(InputDeviceKey::LEFT_ARROW);
    }
    AssertHelper::assertStringEquals(textBox->getTextWidget().getBaseText(), "abcdef");
}

void TextBoxTest::textSelection() {
    auto uiRenderer = setupUiRenderer();
    auto textBox = TextBox::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(50.0f, 20.0f, PIXEL), "test");
    uiRenderer->addWidget(textBox);

    std::string textValue = "0123456789"; //text box can only display the text partially
    uiRenderer->onMouseMove(1.0, 1.0, 0.0, 0.0); //move mouse over text box
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT); //activate text box
    for (char textLetter : textValue) {
        uiRenderer->onChar(static_cast<char32_t>(textLetter));
    }

    for (std::size_t i = 0; i < textValue.size(); ++i) {
        uiRenderer->onKeyPress(InputDeviceKey::LEFT_ARROW);
    }
    AssertHelper::assertStringEquals(textBox->getTextWidget().getBaseText(), "012345");

    uiRenderer->onKeyPress(InputDeviceKey::CTRL);
    uiRenderer->onKeyPress(InputDeviceKey::A); //select all
    uiRenderer->onKeyRelease(InputDeviceKey::CTRL);
    AssertHelper::assertStringEquals(textBox->getTextWidget().getBaseText(), "56789");

    uiRenderer->onKeyPress(InputDeviceKey::DELETE_KEY); //delete selection
    AssertHelper::assertStringEquals(textBox->getTextWidget().getBaseText(), "");
}

std::unique_ptr<UIRenderer> TextBoxTest::setupUiRenderer() {
    renderTarget = std::make_unique<NullRenderTarget>(1920, 1080);
    i18nService = std::make_unique<I18nService>();
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    return std::make_unique<UIRenderer>(1.0f, *renderTarget, *i18nService);
}

CppUnit::Test* TextBoxTest::suite() {
    auto* suite = new CppUnit::TestSuite("TextBoxTest");

    suite->addTest(new CppUnit::TestCaller<TextBoxTest>("textShift", &TextBoxTest::textShift));
    suite->addTest(new CppUnit::TestCaller<TextBoxTest>("textSelection", &TextBoxTest::textSelection));

    return suite;
}