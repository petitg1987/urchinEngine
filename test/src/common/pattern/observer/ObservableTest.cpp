#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "AssertHelper.h"
#include "common/pattern/observer/ObservableTest.h"
using namespace urchin;

void MyObserver::notify(urchin::Observable* observable, int notificationType) {
    receivedNotifications.push_back(std::make_pair(observable, notificationType));
}

void ObservableTest::notifyObservers() {
    MyObservable myObservable;
    MyObserver myObserver1;
    myObservable.addObserver(&myObserver1, 0);
    MyObserver myObserver2;
    myObservable.addObserver(&myObserver2, 1);

    myObservable.notifyObservers(&myObservable, 0);

    AssertHelper::assertUnsignedIntEquals(myObserver1.receivedNotifications.size(), 1l);
    AssertHelper::assertTrue(myObserver1.receivedNotifications[0].first == &myObservable);
    AssertHelper::assertIntEquals(myObserver1.receivedNotifications[0].second, 0);
    AssertHelper::assertTrue(myObserver2.receivedNotifications.empty());
}

CppUnit::Test* ObservableTest::suite() {
    auto* suite = new CppUnit::TestSuite("ObservableTest");

    suite->addTest(new CppUnit::TestCaller("notifyObservers", &ObservableTest::notifyObservers));

    return suite;
}
