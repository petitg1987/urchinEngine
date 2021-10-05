#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <physics/character/CharacterControllerIT.h>
#include <physics/character/CharacterControllerMT.h>
using namespace urchin;

CppUnit::Test* CharacterControllerMT::suite() {
    auto* suite = new CppUnit::TestSuite("CharacterControllerMT");

    for (unsigned int i = 0; i < 100; ++i) {
        suite->addTest(new CppUnit::TestCaller<CharacterControllerIT>("fallingCharacterOnObjects_" + std::to_string(i), &CharacterControllerIT::fallingCharacterOnObjects));
    }

    for (unsigned int i = 0; i < 100; ++i) {
        suite->addTest(new CppUnit::TestCaller<CharacterControllerIT>("characterMovingOnRemovedObjects_" + std::to_string(i), &CharacterControllerIT::characterMovingOnRemovedObjects));
    }

    return suite;
}