#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinSoundEngine.h>

#include <AssertHelper.h>
#include <sound/player/filereader/SoundFileReaderTest.h>
using namespace urchin;

void SoundFileReaderTest::cursorAfterRead() {
    SoundFileReader soundFileReader(FileSystem::instance().getResourcesDirectory() + "sound/sound5SecStereo.ogg");
    std::size_t dataSize = 5000;

    std::vector<int16_t> data;
    data.resize(dataSize);
    unsigned int numSamplesRead = 0;
    soundFileReader.readNextChunk(data, numSamplesRead, true);

    AssertHelper::assertUnsignedIntEquals(numSamplesRead, (unsigned int)data.size());
    AssertHelper::assertUnsignedIntEquals(numSamplesRead, soundFileReader.getNumSamplesRead());
}

void SoundFileReaderTest::cursorAfterMove() {
    SoundFileReader soundFileReader(FileSystem::instance().getResourcesDirectory() + "sound/sound5SecStereo.ogg");
    unsigned int advanceSize = 5000;

    soundFileReader.advanceReadCursor(advanceSize, true);
    AssertHelper::assertUnsignedIntEquals(soundFileReader.getNumSamplesRead(), advanceSize);

    soundFileReader.advanceReadCursor(advanceSize, true);
    AssertHelper::assertUnsignedIntEquals(soundFileReader.getNumSamplesRead(), advanceSize * 2);
}

CppUnit::Test* SoundFileReaderTest::suite() {
    auto* suite = new CppUnit::TestSuite("SoundFileReaderTest");

    suite->addTest(new CppUnit::TestCaller("cursorAfterRead", &SoundFileReaderTest::cursorAfterRead));
    suite->addTest(new CppUnit::TestCaller("cursorAfterMove", &SoundFileReaderTest::cursorAfterMove));

    return suite;
}
