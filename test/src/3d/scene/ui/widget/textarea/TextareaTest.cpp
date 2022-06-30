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
    uiRenderer->onMouseMove(1.0f, 1.0f); //move mouse over textarea
    uiRenderer->onKeyPress((int)InputDeviceKey::MOUSE_LEFT); //activate textarea
    for (char textLetter : textValue) {
        uiRenderer->onChar(static_cast<char32_t>(textLetter));
    }
    AssertHelper::assertUnsignedIntEquals(textarea->getTextWidget().getCutTextLines().size(), 2);
    AssertHelper::assertTrue(textarea->getTextWidget().getCutTextLines()[0].text == WStringConvertA().from_bytes("mmm"));
    AssertHelper::assertTrue(textarea->getTextWidget().getCutTextLines()[1].text == WStringConvertA().from_bytes("mmm"));

    float endOfLinePosX = textarea->getWidth() - (float)textarea->getOutline().rightWidth - 10.0f /* scrollbar width */ - TextFieldConst::TEXT_AND_SCROLLBAR_SHIFT;
    uiRenderer->onMouseMove(endOfLinePosX, 1.0f); //move mouse at end of first line
    uiRenderer->onKeyPress((int)InputDeviceKey::MOUSE_LEFT); //place cursor at end of first line
    for (std::size_t i = 0; i < 3; ++i) {
        uiRenderer->onKeyPress((int)InputDeviceKey::DELETE_KEY);
    }
    AssertHelper::assertUnsignedIntEquals(textarea->getTextWidget().getCutTextLines().size(), 1);
    AssertHelper::assertTrue(textarea->getTextWidget().getCutTextLines()[0].text == WStringConvertA().from_bytes("mmm"));

    uiRenderer->onChar('m');
    AssertHelper::assertUnsignedIntEquals(textarea->getTextWidget().getCutTextLines().size(), 2);
}

std::unique_ptr<UIRenderer> TextareaTest::setupUiRenderer() {
    renderTarget = std::make_unique<NullRenderTarget>(1920, 1080);
    i18nService = std::make_unique<I18nService>();
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    return std::make_unique<UIRenderer>(*renderTarget, *i18nService);
}

CppUnit::Test* TextareaTest::suite() {
    auto* suite = new CppUnit::TestSuite("TextareaTest");

    suite->addTest(new CppUnit::TestCaller<TextareaTest>("textCut", &TextareaTest::textCut));

    return suite;
}