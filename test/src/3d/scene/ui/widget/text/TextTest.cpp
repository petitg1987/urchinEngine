#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "3d/scene/ui/widget/text/TextTest.h"
#include "AssertHelper.h"
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

    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(4), 4); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(7), 7); //last char

    AssertHelper::assertUnsignedIntEquals(text->cutTextIndexToBaseTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->cutTextIndexToBaseTextIndex(4), 4); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->cutTextIndexToBaseTextIndex(7), 7); //last char

    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(2), 3); //second char of first line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(3), 3); //last char of first line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(4), 7); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(7), 7); //last char of second line
}

void TextTest::cutWord() {
    auto uiRenderer = setupUiRenderer();
    auto text = Text::create(nullptr, Position(0.0f, 0.0f, PIXEL), "test", "aaa bbb");
    text->setMaxWidth(28.0f, PIXEL); //cut after 'aaa'
    uiRenderer->addWidget(text);

    AssertHelper::assertUnsignedIntEquals(text->getCutTextLines().size(), 2);
    AssertHelper::assertTrue(text->getCutTextLines()[0].text == WStringConvertA().from_bytes("aaa"));
    AssertHelper::assertTrue(text->getCutTextLines()[0].cutType == TextCutType::WORD);
    AssertHelper::assertTrue(text->getCutTextLines()[1].text == WStringConvertA().from_bytes("bbb"));
    AssertHelper::assertTrue(text->getCutTextLines()[1].cutType == TextCutType::CLASSIC);

    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(3), 3); //last char of first line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(4), 4); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(7), 7); //last char

    AssertHelper::assertUnsignedIntEquals(text->cutTextIndexToBaseTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->cutTextIndexToBaseTextIndex(3), 3); //last char of first line
    AssertHelper::assertUnsignedIntEquals(text->cutTextIndexToBaseTextIndex(4), 4); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->cutTextIndexToBaseTextIndex(7), 7); //last char

    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(2), 3); //second char of first line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(3), 3); //last char of first line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(4), 7); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(7), 7); //last char of second line
}

void TextTest::cutMiddleWord() {
    auto uiRenderer = setupUiRenderer();
    auto text = Text::create(nullptr, Position(0.0f, 0.0f, PIXEL), "test", "aaabbb");
    text->setMaxWidth(20.0f, PIXEL); //cut after 'aaa' (letter 'a' width: 5 pixel, letter 'b' width: 6 pixel)
    uiRenderer->addWidget(text);

    AssertHelper::assertUnsignedIntEquals(text->getCutTextLines().size(), 2);
    AssertHelper::assertTrue(text->getCutTextLines()[0].text == WStringConvertA().from_bytes("aaa"));
    AssertHelper::assertTrue(text->getCutTextLines()[0].cutType == TextCutType::MIDDLE_WORD);
    AssertHelper::assertTrue(text->getCutTextLines()[1].text == WStringConvertA().from_bytes("bbb"));
    AssertHelper::assertTrue(text->getCutTextLines()[1].cutType == TextCutType::CLASSIC);

    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(2), 2); //third char
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(3, WordCutIndexPositioning::BEGIN_OF_NEXT_LINE), 4); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(3, WordCutIndexPositioning::END_OF_LINE), 3); //last char of first line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(6, WordCutIndexPositioning::BEGIN_OF_NEXT_LINE), 7); //last char
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToCutTextIndex(6, WordCutIndexPositioning::END_OF_LINE), 7); //last char

    AssertHelper::assertUnsignedIntEquals(text->cutTextIndexToBaseTextIndex(0), 0); //first char
    AssertHelper::assertUnsignedIntEquals(text->cutTextIndexToBaseTextIndex(3), 3); //first char of second line (cut text index 3 is identical to cut text index 4)
    AssertHelper::assertUnsignedIntEquals(text->cutTextIndexToBaseTextIndex(4), 3); //first char of second line (cut text index 4 is identical to cut text index 3)
    AssertHelper::assertUnsignedIntEquals(text->cutTextIndexToBaseTextIndex(7), 6); //last char

    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(2), 3); //second char of first line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(3, WordCutIndexPositioning::BEGIN_OF_NEXT_LINE), 6); //first char of second line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(3, WordCutIndexPositioning::END_OF_LINE), 3); //last char of first line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(4), 6); //second char of second line
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(6), 6); //last char of second line
}

void TextTest::emptyLineAndCut() {
    auto uiRenderer = setupUiRenderer();
    auto text = Text::create(nullptr, Position(0.0f, 0.0f, PIXEL), "test", "\naaabbb");
    text->setMaxWidth(20.0f, PIXEL); //cut after 'aaa' (letter 'a' width: 5 pixel, letter 'b' width: 6 pixel)
    uiRenderer->addWidget(text);

    AssertHelper::assertUnsignedIntEquals(text->getCutTextLines().size(), 3);
    AssertHelper::assertUnsignedIntEquals(text->getCutTextLines()[0].text.length(), 0);
    AssertHelper::assertTrue(text->getCutTextLines()[1].text == WStringConvertA().from_bytes("aaa"));
    AssertHelper::assertTrue(text->getCutTextLines()[2].text == WStringConvertA().from_bytes("bbb"));

    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(0), 0);
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(1), 4);
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(4, WordCutIndexPositioning::BEGIN_OF_NEXT_LINE), 7);
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(4, WordCutIndexPositioning::END_OF_LINE), 4);
    AssertHelper::assertUnsignedIntEquals(text->baseTextIndexToEndOfLineIndex(5), 7);
}

std::unique_ptr<UIRenderer> TextTest::setupUiRenderer() {
    renderTarget = std::make_unique<OffscreenRender>("test", true, RenderTarget::NO_DEPTH_ATTACHMENT);
    renderTarget->setOutputSize(1920, 1080, 1, false);
    i18nService = std::make_unique<I18nService>();
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    return std::make_unique<UIRenderer>(1.0f, *renderTarget, *i18nService);
}

CppUnit::Test* TextTest::suite() {
    auto* suite = new CppUnit::TestSuite("TextTest");

    suite->addTest(new CppUnit::TestCaller("twoLines", &TextTest::twoLines));
    suite->addTest(new CppUnit::TestCaller("cutWord", &TextTest::cutWord));
    suite->addTest(new CppUnit::TestCaller("cutMiddleWord", &TextTest::cutMiddleWord));
    suite->addTest(new CppUnit::TestCaller("emptyLineAndCut", &TextTest::emptyLineAndCut));

    return suite;
}