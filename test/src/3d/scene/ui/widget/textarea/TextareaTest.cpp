#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/ui/widget/textarea/TextareaTest.h>
#include <AssertHelper.h>
using namespace urchin;

void TextareaTest::textCut() {
    auto uiRenderer = setupUiRenderer();
    auto textarea = Textarea::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(50.0f, 100.0f, PIXEL), "test");
    uiRenderer->addWidget(textarea);

    std::string textValue = "mmmmmm"; //textarea can only display 'mmm' on one line
    uiRenderer->onMouseMove(1.0, 1.0, 0.0, 0.0); //move mouse over textarea
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT); //activate textarea
    for (char textLetter : textValue) {
        uiRenderer->onChar(static_cast<char32_t>(textLetter));
    }
    AssertHelper::assertUnsignedIntEquals(textarea->getTextWidget().getCutTextLines().size(), 2);
    AssertHelper::assertTrue(textarea->getTextWidget().getCutTextLines()[0].text == WStringConvertA().from_bytes("mmm"));
    AssertHelper::assertTrue(textarea->getTextWidget().getCutTextLines()[1].text == WStringConvertA().from_bytes("mmm"));

    float endOfLinePosX = textarea->getWidth() - 1.0f /* outline right */ - 10.0f /* scrollbar width */ - TextFieldConst::TEXT_AND_SCROLLBAR_SHIFT;
    uiRenderer->onMouseMove(endOfLinePosX, 1.0, 0.0, 0.0); //move mouse at end of first line
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT); //place cursor at end of first line
    for (std::size_t i = 0; i < 3; ++i) {
        uiRenderer->onKeyPress(InputDeviceKey::DELETE_KEY);
    }
    AssertHelper::assertUnsignedIntEquals(textarea->getTextWidget().getCutTextLines().size(), 1);
    AssertHelper::assertTrue(textarea->getTextWidget().getCutTextLines()[0].text == WStringConvertA().from_bytes("mmm"));

    uiRenderer->onChar('m');
    AssertHelper::assertUnsignedIntEquals(textarea->getTextWidget().getCutTextLines().size(), 2);
    AssertHelper::assertTrue(textarea->getTextWidget().getCutTextLines()[0].text == WStringConvertA().from_bytes("mmm"));
    AssertHelper::assertTrue(textarea->getTextWidget().getCutTextLines()[1].text == WStringConvertA().from_bytes("m"));
}

void TextareaTest::textCopyPaste() {
    auto uiRenderer = setupUiRenderer();
    auto textarea = Textarea::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(500.0f, 100.0f, PIXEL), "test");
    uiRenderer->addWidget(textarea);

    std::string textValue = "123";
    uiRenderer->onMouseMove(1.0, 1.0, 0.0, 0.0); //move mouse over textarea
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT); //activate textarea
    for (char textLetter : textValue) {
        uiRenderer->onChar(static_cast<char32_t>(textLetter));
    }
    float endOfLinePosX = textarea->getWidth() - 1.0f /* outline right */ - 10.0f /* scrollbar width */ - TextFieldConst::TEXT_AND_SCROLLBAR_SHIFT;
    uiRenderer->onKeyPress(InputDeviceKey::CTRL);
    uiRenderer->onKeyPress(InputDeviceKey::A); //select all
    uiRenderer->onKeyPress(InputDeviceKey::C); //copy
    uiRenderer->onKeyRelease(InputDeviceKey::CTRL);
    uiRenderer->onMouseMove(endOfLinePosX, 1.0f, 0.0, 0.0); //move mouse at end of first line
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT); //place cursor at end of first line
    uiRenderer->onKeyPress(InputDeviceKey::CTRL);
    uiRenderer->onKeyPress(InputDeviceKey::V); //paste
    uiRenderer->onKeyRelease(InputDeviceKey::CTRL);

    AssertHelper::assertStringEquals(textarea->getText(), "123123");
}

void TextareaTest::leftArrowWithSelection() {
    auto uiRenderer = setupUiRenderer();
    auto textarea = Textarea::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(500.0f, 100.0f, PIXEL), "test");
    uiRenderer->addWidget(textarea);

    uiRenderer->onMouseMove(1.0, 1.0, 0.0, 0.0); //move mouse over textarea
    uiRenderer->onKeyPress(InputDeviceKey::MOUSE_LEFT); //activate textarea
    uiRenderer->onChar(static_cast<char32_t>('a'));
    uiRenderer->onChar(static_cast<char32_t>('b'));
    uiRenderer->onKeyPress(InputDeviceKey::CTRL);
    uiRenderer->onKeyPress(InputDeviceKey::A); //select all
    uiRenderer->onKeyRelease(InputDeviceKey::CTRL);
    uiRenderer->onKeyPress(InputDeviceKey::LEFT_ARROW); //cursor index at 0
    uiRenderer->onChar(static_cast<char32_t>('c'));

    AssertHelper::assertStringEquals(textarea->getText(), "cab");
}

std::unique_ptr<UIRenderer> TextareaTest::setupUiRenderer() {
    renderTarget = std::make_unique<NullRenderTarget>(1920, 1080);
    i18nService = std::make_unique<I18nService>();
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    return std::make_unique<UIRenderer>(1.0f, *renderTarget, *i18nService);
}

CppUnit::Test* TextareaTest::suite() {
    auto* suite = new CppUnit::TestSuite("TextareaTest");

    suite->addTest(new CppUnit::TestCaller<TextareaTest>("textCut", &TextareaTest::textCut));
    suite->addTest(new CppUnit::TestCaller<TextareaTest>("textCopyPaste", &TextareaTest::textCopyPaste));
    suite->addTest(new CppUnit::TestCaller<TextareaTest>("leftArrowWithSelection", &TextareaTest::leftArrowWithSelection));

    return suite;
}