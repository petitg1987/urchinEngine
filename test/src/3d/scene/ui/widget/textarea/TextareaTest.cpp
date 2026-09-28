#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "3d/scene/ui/widget/textarea/TextareaTest.h"
#include "AssertHelper.h"
using namespace urchin;

void TextareaTest::textCut() {
    auto uiRenderer = setupUiRenderer();
    auto textarea = Textarea::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(40.0f, 100.0f, PIXEL), "test");
    uiRenderer->addWidget(textarea);

    std::string textValue = "mmmmmm"; //textarea can only display 'mmm' on one line
    uiRenderer->onMouseMove(1.0, 1.0, 0.0, 0.0); //move mouse over textarea
    uiRenderer->onKeyPress(InputKey::LMB); //activate textarea
    for (char textLetter : textValue) {
        uiRenderer->onChar(static_cast<char32_t>(textLetter));
    }
    AssertHelper::assertUnsignedIntEquals(textarea->getTextWidget().getCutTextLines().size(), 2);
    AssertHelper::assertTrue(StringUtil::readUtf8String(textarea->getTextWidget().getCutTextLines()[0].text) == "mmm");
    AssertHelper::assertTrue(StringUtil::readUtf8String(textarea->getTextWidget().getCutTextLines()[1].text) == "mmm");

    float endOfLinePosX = textarea->getWidth() - 1.0f /* outline right */ - 10.0f /* scrollbar width */ - TextFieldConst::TEXT_AND_SCROLLBAR_SHIFT;
    uiRenderer->onMouseMove(endOfLinePosX, 1.0, 0.0, 0.0); //move mouse at end of first line
    uiRenderer->onKeyPress(InputKey::LMB); //place cursor at end of first line
    for (std::size_t i = 0; i < 3; ++i) {
        uiRenderer->onKeyPress(InputKey::DEL);
    }
    AssertHelper::assertUnsignedIntEquals(textarea->getTextWidget().getCutTextLines().size(), 1);
    AssertHelper::assertTrue(StringUtil::readUtf8String(textarea->getTextWidget().getCutTextLines()[0].text) == "mmm");

    uiRenderer->onChar('m');
    AssertHelper::assertUnsignedIntEquals(textarea->getTextWidget().getCutTextLines().size(), 2);
    AssertHelper::assertTrue(StringUtil::readUtf8String(textarea->getTextWidget().getCutTextLines()[0].text) == "mmm");
    AssertHelper::assertTrue(StringUtil::readUtf8String(textarea->getTextWidget().getCutTextLines()[1].text) == "m");
}

void TextareaTest::textCopyPaste() {
    auto uiRenderer = setupUiRenderer();
    auto textarea = Textarea::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(500.0f, 100.0f, PIXEL), "test");
    uiRenderer->addWidget(textarea);

    std::string textValue = "123";
    uiRenderer->onMouseMove(1.0, 1.0, 0.0, 0.0); //move mouse over textarea
    uiRenderer->onKeyPress(InputKey::LMB); //activate textarea
    for (char textLetter : textValue) {
        uiRenderer->onChar(static_cast<char32_t>(textLetter));
    }
    float endOfLinePosX = textarea->getWidth() - 1.0f /* outline right */ - 10.0f /* scrollbar width */ - TextFieldConst::TEXT_AND_SCROLLBAR_SHIFT;
    uiRenderer->onKeyPress(InputKey::CTRL_LEFT);
    uiRenderer->onKeyPress(InputKey::A); //select all
    uiRenderer->onKeyPress(InputKey::C); //copy
    uiRenderer->onKeyRelease(InputKey::CTRL_LEFT);
    uiRenderer->onMouseMove(endOfLinePosX, 1.0f, 0.0, 0.0); //move mouse at end of first line
    uiRenderer->onKeyPress(InputKey::LMB); //place cursor at end of first line
    uiRenderer->onKeyPress(InputKey::CTRL_LEFT);
    uiRenderer->onKeyPress(InputKey::V); //paste
    uiRenderer->onKeyRelease(InputKey::CTRL_LEFT);

    AssertHelper::assertStringEquals(textarea->getText(), "123123");
}

void TextareaTest::leftArrowWithSelection() {
    auto uiRenderer = setupUiRenderer();
    auto textarea = Textarea::create(nullptr, Position(0.0f, 0.0f, PIXEL), Size(500.0f, 100.0f, PIXEL), "test");
    uiRenderer->addWidget(textarea);

    uiRenderer->onMouseMove(1.0, 1.0, 0.0, 0.0); //move mouse over textarea
    AssertHelper::assertFalse(uiRenderer->onKeyPress(InputKey::LMB)); //activate textarea
    uiRenderer->onChar('a');
    uiRenderer->onChar('b');
    AssertHelper::assertTrue(uiRenderer->onKeyPress(InputKey::CTRL_LEFT));
    AssertHelper::assertFalse(uiRenderer->onKeyPress(InputKey::A)); //select all
    AssertHelper::assertTrue(uiRenderer->onKeyRelease(InputKey::CTRL_LEFT));
    AssertHelper::assertFalse(uiRenderer->onKeyPress(InputKey::ARROW_LEFT)); //cursor index at 0
    AssertHelper::assertTrue(uiRenderer->onKeyPress(InputKey::ARROW_LEFT)); //cursor index still at 0
    uiRenderer->onChar('c');

    AssertHelper::assertStringEquals(textarea->getText(), "cab");
}

std::unique_ptr<UIRenderer> TextareaTest::setupUiRenderer() {
    renderTarget = std::make_unique<OffscreenRender>("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    renderTarget->setOutputSize(1920, 1080, 1, false);
    i18nService = std::make_unique<I18nService>();
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    return std::make_unique<UIRenderer>(1.0f, *renderTarget, *i18nService);
}

CppUnit::Test* TextareaTest::suite() {
    auto* suite = new CppUnit::TestSuite("TextareaTest");

    suite->addTest(new CppUnit::TestCaller("textCut", &TextareaTest::textCut));
    suite->addTest(new CppUnit::TestCaller("textCopyPaste", &TextareaTest::textCopyPaste));
    suite->addTest(new CppUnit::TestCaller("leftArrowWithSelection", &TextareaTest::leftArrowWithSelection));

    return suite;
}