#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>

#include <common/system/SystemInfoTest.h>
#include <AssertHelper.h>
using namespace urchin;

void SystemInfoTest::retrieveOsInfo() {
    std::string osInfo = SystemInfo::retrieveOsInfo();

    AssertHelper::assertTrue(!osInfo.empty(), "OS info cannot be empty");
}

void SystemInfoTest::retrieveCpuCores() {
    unsigned int cpuCores = SystemInfo::retrieveCpuCores();

    AssertHelper::assertTrue(cpuCores > 0 && cpuCores < 200, "Wrong number of CPU cores");
}

void SystemInfoTest::retrieveTotalMemory() {
    uint64_t totalMemory = SystemInfo::retrieveTotalMemory();

    AssertHelper::assertTrue(totalMemory > 4000000000 && totalMemory < 512000000000, "Wrong total of memory");
}

void SystemInfoTest::retrieveGraphicsCardNames() {
    std::string graphicsCardNames = SystemInfo::retrieveGraphicsCardNames();

    AssertHelper::assertTrue(!graphicsCardNames.empty(), "Graphics card name cannot be empty");
}

CppUnit::Test* SystemInfoTest::suite() {
    auto* suite = new CppUnit::TestSuite("SystemInfoTest");

    suite->addTest(new CppUnit::TestCaller("retrieveOsInfo", &SystemInfoTest::retrieveOsInfo));
    suite->addTest(new CppUnit::TestCaller("retrieveCpuCores", &SystemInfoTest::retrieveCpuCores));
    suite->addTest(new CppUnit::TestCaller("retrieveTotalMemory", &SystemInfoTest::retrieveTotalMemory));
    suite->addTest(new CppUnit::TestCaller("retrieveGraphicsCardNames", &SystemInfoTest::retrieveGraphicsCardNames));

    return suite;
}
