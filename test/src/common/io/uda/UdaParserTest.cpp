#include <cppunit/extensions/HelperMacros.h>

#include <common/io/uda/UdaParserTest.h>
#include <AssertHelper.h>
using namespace urchin;

void UdaParserTest::removeChunk() {
    std::string filename = FileSystem::instance().getResourcesDirectory() + "test.uda";
    UdaParser udaParserWriter(filename, UdaLoadType::NO_LOAD);
    UdaChunk& main = udaParserWriter.createChunk("main", UdaAttribute(), nullptr);
    udaParserWriter.createChunk("child1", UdaAttribute(), &main).setStringValue("child1");
    udaParserWriter.createChunk("child2", UdaAttribute(), &main).setStringValue("child2");
    udaParserWriter.createChunk("child3", UdaAttribute(), &main).setStringValue("child3");
    UdaChunk* child2Retrieved = udaParserWriter.getFirstChunk(true, "child2", UdaAttribute(), &main);

    udaParserWriter.removeChunk(*child2Retrieved);
    udaParserWriter.save();

    UdaParser udaParserReader(filename, UdaLoadType::LOAD_FILE);
    AssertHelper::assertObjectEquals(udaParserReader.getFirstChunk(false, "child2", UdaAttribute(), &main), nullptr);
    AssertHelper::assertIntEquals(std::remove(filename.c_str()), 0);
}

CppUnit::Test* UdaParserTest::suite() {
    auto* suite = new CppUnit::TestSuite("UdaParserTest");

    suite->addTest(new CppUnit::TestCaller<UdaParserTest>("removeChunk", &UdaParserTest::removeChunk));

    return suite;
}
