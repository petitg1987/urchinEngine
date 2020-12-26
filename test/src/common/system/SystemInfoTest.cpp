#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"

#include "SystemInfoTest.h"
#include "AssertHelper.h"
using namespace urchin;

void SystemInfoTest::retrieveOsInfo() {
    std::string osInfo = SystemInfo::instance()->retrieveOsInfo();

    AssertHelper::instance()->assertTrue(!osInfo.empty(), "OS info cannot be empty");
}

void SystemInfoTest::retrieveCpuCores() {
    unsigned int cpuCores = SystemInfo::instance()->retrieveCpuCores();

    AssertHelper::instance()->assertTrue(cpuCores > 0 && cpuCores < 200, "Wrong number of CPU cores");
}

void SystemInfoTest::retrieveTotalMemory() {
    uint64_t totalMemory = SystemInfo::instance()->retrieveTotalMemory();

    AssertHelper::instance()->assertTrue(totalMemory > 4000000000 && totalMemory < 512000000000, "Wrong total of memory");
}

void SystemInfoTest::retrieveGraphicsCardNames() {
    std::string graphicsCardNames = SystemInfo::instance()->retrieveGraphicsCardNames();

    AssertHelper::instance()->assertTrue(!graphicsCardNames.empty(), "Graphics card name cannot be empty");
}

void SystemInfoTest::systemHash() {
    std::string systemHash1 = SystemInfo::instance()->systemHash();
    std::string systemHash2 = SystemInfo::instance()->systemHash();

    AssertHelper::instance()->assertTrue(!systemHash1.empty(), "System hash cannot be empty");
    AssertHelper::instance()->assertString(systemHash1, systemHash2);
}

CppUnit::Test* SystemInfoTest::suite() {
    auto* suite = new CppUnit::TestSuite("SystemInfoTest");

    suite->addTest(new CppUnit::TestCaller<SystemInfoTest>("retrieveOsInfo", &SystemInfoTest::retrieveOsInfo));
    suite->addTest(new CppUnit::TestCaller<SystemInfoTest>("retrieveCpuCores", &SystemInfoTest::retrieveCpuCores));
    suite->addTest(new CppUnit::TestCaller<SystemInfoTest>("retrieveTotalMemory", &SystemInfoTest::retrieveTotalMemory));
    suite->addTest(new CppUnit::TestCaller<SystemInfoTest>("retrieveGraphicsCardNames", &SystemInfoTest::retrieveGraphicsCardNames));

    suite->addTest(new CppUnit::TestCaller<SystemInfoTest>("systemHash", &SystemInfoTest::systemHash));

    return suite;
}
