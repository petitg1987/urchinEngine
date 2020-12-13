#include <cppunit/ui/text/TestRunner.h>
#include "UrchinCommon.h"

int main() {
    urchin::ConfigService::instance()->loadProperties("resources/engine.properties");

    CppUnit::TextUi::TestRunner runner;
    //TODO add tests
    runner.run();

    urchin::SingletonManager::destroyAllSingletons();
    return 0;
}
