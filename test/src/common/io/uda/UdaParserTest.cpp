#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/io/uda/UdaParserTest.h>
#include <AssertHelper.h>
using namespace urchin;

void UdaParserTest::removeChunk() {
    std::string filename = FileSystem::instance().getResourcesDirectory() + "test.uda";
    UdaParser udaParserWriter(filename, UdaLoadType::NO_LOAD);
    UdaChunk& newMain = udaParserWriter.createChunk("main", UdaAttribute(), nullptr);
    udaParserWriter.createChunk("child1", UdaAttribute(), &newMain).setStringValue("child1");
    udaParserWriter.createChunk("child2", UdaAttribute(), &newMain).setStringValue("child2");
    udaParserWriter.createChunk("child3", UdaAttribute(), &newMain).setStringValue("child3");
    const UdaChunk* child2Retrieved = udaParserWriter.getFirstChunk(true, "child2", UdaAttribute(), &newMain);

    udaParserWriter.removeChunk(*child2Retrieved);
    udaParserWriter.save();

    UdaParser udaParserReader(filename, UdaLoadType::LOAD_FILE);
    const UdaChunk* main = udaParserReader.getFirstChunk(true, "main", UdaAttribute(), nullptr);
    udaParserReader.getFirstChunk(true, "child1", UdaAttribute(), main); //no exception throw expected
    AssertHelper::assertNull(udaParserReader.getFirstChunk(false, "child2", UdaAttribute(), main));
    udaParserReader.getFirstChunk(true, "child3", UdaAttribute(), main); //no exception throw expected
    AssertHelper::assertIntEquals(std::remove(filename.c_str()), 0);
}

CppUnit::Test* UdaParserTest::suite() {
    auto* suite = new CppUnit::TestSuite("UdaParserTest");

    suite->addTest(new CppUnit::TestCaller("removeChunk", &UdaParserTest::removeChunk));

    return suite;
}
