#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/container/EverGrowQueueTest.h>
#include <AssertHelper.h>
using namespace urchin;

void EverGrowQueueTest::pushAndPop() {
    EverGrowQueue<int> queue(4, 2);

    queue.pushBack(2);
    AssertHelper::assertIntEquals(queue.getSize(), 1);
    AssertHelper::assertIntEquals(queue.getCapacity(), 4);
    AssertHelper::assertIntEquals(queue.popFront(), 2);
    AssertHelper::assertIntEquals(queue.getSize(), 0);

    queue.pushBack(3);
    queue.pushBack(4);
    queue.pushBack(5);
    queue.pushBack(6);
    queue.pushBack(7);
    AssertHelper::assertIntEquals(queue.getSize(), 5);
    AssertHelper::assertIntEquals(queue.getCapacity(), 6);
    AssertHelper::assertIntEquals(queue.popFront(), 3);

    queue.popFront();
    queue.popFront();
    queue.popFront();
    queue.popFront();
    AssertHelper::assertIntEquals(queue.getSize(), 0);
    AssertHelper::assertIntEquals(queue.getCapacity(), 6);

    queue.pushBack(2);
    queue.pushBack(3);
    AssertHelper::assertIntEquals(queue.popBack(), 3);
    AssertHelper::assertIntEquals(queue.popFront(), 2);
    AssertHelper::assertIntEquals(queue.getCapacity(), 6);
}

CppUnit::Test* EverGrowQueueTest::suite() {
    auto* suite = new CppUnit::TestSuite("EverGrowQueueTest");

    suite->addTest(new CppUnit::TestCaller("pushAndPop", &EverGrowQueueTest::pushAndPop));

    return suite;
}