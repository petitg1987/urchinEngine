#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class InputDeviceTrackerTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void switchToGamepadByKey();
        void switchToGamepadByAnalogInput();
        void ignoreSmallAnalogInput();
        void switchBackToKeyboardMouse();
        void ignoreSyntheticMouseMove();
        void ignoreUnknownKey();
        void gamepadType();
};
