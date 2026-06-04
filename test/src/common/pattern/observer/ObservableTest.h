#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

struct MyObservable : urchin::Observable {

};

struct MyObserver : urchin::Observer {
    void notify(urchin::Observable*, int) override;
    std::vector<std::pair<urchin::Observable*, int>> receivedNotifications;
};

class ObservableTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void notifyObservers();
};
