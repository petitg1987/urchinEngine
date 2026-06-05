#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "AssertHelper.h"
#include "common/pattern/observer/ObservableTest.h"
using namespace urchin;

MyObserver::MyObserver(std::function<void(Observable*, int)> notifyCallback) :
        Observer(),
        notifyCallback(std::move(notifyCallback)) {

}

void MyObserver::notify(Observable* observable, int notificationType) {
    notifyCallback(observable, notificationType);
}

void ObservableTest::notifyObservers() {
    int notificationType = 0;
    std::vector<std::pair<Observable*, int>> receivedNotif;
    MyObservable myObservable;
    MyObserver myObserver([&](Observable* o, int nt) { receivedNotif.push_back(std::make_pair(o, nt)); });
    myObservable.addObserver(&myObserver, notificationType);

    myObservable.notifyObservers(&myObservable, notificationType);

    AssertHelper::assertUnsignedIntEquals(receivedNotif.size(), 1l);
    AssertHelper::assertTrue(receivedNotif[0].first == &myObservable);
    AssertHelper::assertIntEquals(receivedNotif[0].second, notificationType);
}

void ObservableTest::notifyObserversWithRemove() {
    int notificationType = 0;
    std::vector<std::pair<Observable*, int>> receivedNotif;
    MyObservable myObservable;
    MyObserver myObserver2([&](Observable* o, int nt) { receivedNotif.push_back(std::make_pair(o, nt)); });
    MyObserver myObserver1([&](Observable*, int) { myObservable.removeObserver(&myObserver2, notificationType); });
    myObservable.addObserver(&myObserver1, notificationType);
    myObservable.addObserver(&myObserver2, notificationType);

    myObservable.notifyObservers(&myObservable, notificationType);

    AssertHelper::assertUnsignedIntEquals(receivedNotif.size(), 0l);
    AssertHelper::assertUnsignedIntEquals(myObservable.getObservers(notificationType).size(), 1l);
}

void ObservableTest::notifyObserversWithAdd() {
    int notificationType = 0;
    std::vector<std::pair<Observable*, int>> receivedNotif;
    MyObservable myObservable;
    MyObserver myObserver2([&](Observable* o, int nt) { receivedNotif.push_back(std::make_pair(o, nt)); });
    MyObserver myObserver1([&](Observable*, int) { myObservable.addObserver(&myObserver2, notificationType); });
    myObservable.addObserver(&myObserver1, notificationType);

    myObservable.notifyObservers(&myObservable, notificationType);
    AssertHelper::assertUnsignedIntEquals(receivedNotif.size(), 0l);
    AssertHelper::assertUnsignedIntEquals(myObservable.getObservers(notificationType).size(), 2l);

    myObservable.notifyObservers(&myObservable, notificationType);
    AssertHelper::assertUnsignedIntEquals(receivedNotif.size(), 1l);
    AssertHelper::assertUnsignedIntEquals(myObservable.getObservers(notificationType).size(), 2l);
}

CppUnit::Test* ObservableTest::suite() {
    auto* suite = new CppUnit::TestSuite("ObservableTest");

    suite->addTest(new CppUnit::TestCaller("notifyObservers", &ObservableTest::notifyObservers));
    suite->addTest(new CppUnit::TestCaller("notifyObserversWithRemove", &ObservableTest::notifyObserversWithRemove));
    suite->addTest(new CppUnit::TestCaller("notifyObserversWithAdd", &ObservableTest::notifyObserversWithAdd));

    return suite;
}
