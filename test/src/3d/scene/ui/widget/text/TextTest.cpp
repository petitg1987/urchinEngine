#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <3d/scene/ui/widget/text/TextTest.h>
#include <AssertHelper.h>
using namespace urchin;

void TextTest::twoLines() {
    auto uiRenderer = setupUiRenderer();
    auto text = Text::create(nullptr, Position(0.0f, 0.0f, PIXEL), "test", "mmm\nmmm");
    uiRenderer->addWidget(text);

    AssertHelper::assertUnsignedIntEquals(text->getCutTextLines().size(), 2);
    AssertHelper::assertTrue(text->getCutTextLines()[0].text == WStringConvertA().from_bytes("mmm"));
    AssertHelper::assertTrue(text->getCutTextLines()[0].cutType == TextCutType::CLASSIC);
    AssertHelper::assertTrue(text->getCutTextLines()[1].text == WStringConvertA().from_bytes("mmm"));
    AssertHelper::assertTrue(text->getCutTextLines()[1].cutType == TextCutType::CLASSIC);

    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(4), 4); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(7), 7); //last char

    AssertHelper::assertUnsignedIntEquals(text->cutTextToBaseTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->cutTextToBaseTextIndex(4), 4); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->cutTextToBaseTextIndex(7), 7); //last char
}

void TextTest::cutWord() {
    auto uiRenderer = setupUiRenderer();
    auto text = Text::create(nullptr, Position(0.0f, 0.0f, PIXEL), "test", "aaa bbb");
    text->setMaxWidth(200, PIXEL); //cut after 'aaa'
    uiRenderer->addWidget(text);

    AssertHelper::assertUnsignedIntEquals(text->getCutTextLines().size(), 2);
    AssertHelper::assertTrue(text->getCutTextLines()[0].text == WStringConvertA().from_bytes("aaa"));
    AssertHelper::assertTrue(text->getCutTextLines()[0].cutType == TextCutType::WORD);
    AssertHelper::assertTrue(text->getCutTextLines()[1].text == WStringConvertA().from_bytes("bbb"));
    AssertHelper::assertTrue(text->getCutTextLines()[1].cutType == TextCutType::CLASSIC);

    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(3), 3); //last char of first line
    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(4), 4); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(7), 7); //last char

    AssertHelper::assertUnsignedIntEquals(text->cutTextToBaseTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->cutTextToBaseTextIndex(3), 3); //last char of first line
    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(4), 4); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->cutTextToBaseTextIndex(7), 7); //last char
}

void TextTest::cutMiddleWord() {
    auto uiRenderer = setupUiRenderer();
    auto text = Text::create(nullptr, Position(0.0f, 0.0f, PIXEL), "test", "aaabbb");
    text->setMaxWidth(200, PIXEL); //cut after 'aaa'
    uiRenderer->addWidget(text);

    AssertHelper::assertUnsignedIntEquals(text->getCutTextLines().size(), 2);
    AssertHelper::assertTrue(text->getCutTextLines()[0].text == WStringConvertA().from_bytes("aaa"));
    AssertHelper::assertTrue(text->getCutTextLines()[0].cutType == TextCutType::MIDDLE_WORD);
    AssertHelper::assertTrue(text->getCutTextLines()[1].text == WStringConvertA().from_bytes("bbb"));
    AssertHelper::assertTrue(text->getCutTextLines()[1].cutType == TextCutType::CLASSIC);

    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(2), 2); //third char
    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(3), 4); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->baseTextToCutTextIndex(6), 7); //last char

    AssertHelper::assertUnsignedIntEquals(text->cutTextToBaseTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->cutTextToBaseTextIndex(3), 3); //first char of second line (cut text index 3 is identical to cut text index 4)
    AssertHelper::assertUnsignedIntEquals(text->cutTextToBaseTextIndex(4), 3); //first char of second line (cut text index 4 is identical to cut text index 3)
    AssertHelper::assertUnsignedIntEquals(text->cutTextToBaseTextIndex(7), 6); //last char
}

void TextTest::emptyLineAndCut() {
    auto uiRenderer = setupUiRenderer();
    auto text = Text::create(nullptr, Position(0.0f, 0.0f, PIXEL), "test", "\naaabbb");
    text->setMaxWidth(200, PIXEL); //cut after 'aaa'
    uiRenderer->addWidget(text);

    AssertHelper::assertUnsignedIntEquals(text->getCutTextLines().size(), 3);
    AssertHelper::assertUnsignedIntEquals(text->getCutTextLines()[0].text.length(), 0);
    AssertHelper::assertTrue(text->getCutTextLines()[1].text == WStringConvertA().from_bytes("aaa"));
    AssertHelper::assertTrue(text->getCutTextLines()[2].text == WStringConvertA().from_bytes("bbb"));
}

std::unique_ptr<UIRenderer> TextTest::setupUiRenderer() {
    renderTarget = std::make_unique<NullRenderTarget>(1920, 1080);
    i18nService = std::make_unique<I18nService>();
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    return std::make_unique<UIRenderer>(*renderTarget, *i18nService);
}

CppUnit::Test* TextTest::suite() {
    auto* suite = new CppUnit::TestSuite("TextTest");

    suite->addTest(new CppUnit::TestCaller<TextTest>("twoLines", &TextTest::twoLines));
    suite->addTest(new CppUnit::TestCaller<TextTest>("cutWord", &TextTest::cutWord));
    suite->addTest(new CppUnit::TestCaller<TextTest>("cutMiddleWord", &TextTest::cutMiddleWord));
    suite->addTest(new CppUnit::TestCaller<TextTest>("emptyLineAndCut", &TextTest::emptyLineAndCut));

    return suite;
}