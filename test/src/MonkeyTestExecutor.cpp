#include <cppunit/ui/text/TestRunner.h>
#include "UrchinCommon.h"

#include "physics/GhostBodyMT.h"

int main() {
    urchin::ConfigService::instance()->loadProperties("resources/engine.properties");

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(GhostBodyMT::suite());
    runner.run();

    urchin::SingletonManager::destroyAllSingletons();
    return 0;
}
