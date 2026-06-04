#pragma once

#include <functional>
#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

class MyObservable : public urchin::Observable {

};

class MyObserver : public urchin::Observer {
    public:
        explicit MyObserver(std::function<void(urchin::Observable*, int)>);
        void notify(urchin::Observable*, int) override;

    private:
        std::function<void(urchin::Observable*, int)> notifyCallback;
};

class ObservableTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void notifyObservers();
        void notifyObserversWithRemove();
        void notifyObserversWithAdd();
};
