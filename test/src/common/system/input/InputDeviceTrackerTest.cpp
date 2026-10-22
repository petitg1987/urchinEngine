#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>

#include "common/system/input/InputDeviceTrackerTest.h"
#include "AssertHelper.h"
using namespace urchin;

void InputDeviceTrackerTest::switchToGamepadByKey() {
    InputDeviceTracker inputDeviceTracker;

    inputDeviceTracker.onKeyPress(InputKey::GAMEPAD_BUTTON_SOUTH);

    AssertHelper::assertTrue(inputDeviceTracker.getLastInputDevice() == InputDevice::GAMEPAD);
}

void InputDeviceTrackerTest::switchToGamepadByAnalogInput() {
    InputDeviceTracker inputDeviceTracker;

    inputDeviceTracker.onGamepadAnalogInputUpdate(-0.8f);

    AssertHelper::assertTrue(inputDeviceTracker.getLastInputDevice() == InputDevice::GAMEPAD);
}

void InputDeviceTrackerTest::ignoreSmallAnalogInput() {
    InputDeviceTracker inputDeviceTracker;

    inputDeviceTracker.onGamepadAnalogInputUpdate(0.1f);

    AssertHelper::assertTrue(inputDeviceTracker.getLastInputDevice() == InputDevice::KEYBOARD_MOUSE);
}

void InputDeviceTrackerTest::switchBackToKeyboardMouse() {
    InputDeviceTracker inputDeviceTracker;

    inputDeviceTracker.onKeyPress(InputKey::GAMEPAD_BUTTON_SOUTH);
    inputDeviceTracker.onMouseMove(2.0, 0.0);
    AssertHelper::assertTrue(inputDeviceTracker.getLastInputDevice() == InputDevice::KEYBOARD_MOUSE, "Mouse move should switch to keyboard/mouse");

    inputDeviceTracker.onKeyPress(InputKey::GAMEPAD_BUTTON_SOUTH);
    inputDeviceTracker.onScroll();
    AssertHelper::assertTrue(inputDeviceTracker.getLastInputDevice() == InputDevice::KEYBOARD_MOUSE, "Scroll should switch to keyboard/mouse");

    inputDeviceTracker.onKeyPress(InputKey::GAMEPAD_BUTTON_SOUTH);
    inputDeviceTracker.onKeyPress(InputKey::SPACE);
    AssertHelper::assertTrue(inputDeviceTracker.getLastInputDevice() == InputDevice::KEYBOARD_MOUSE, "Keyboard key should switch to keyboard/mouse");
}

void InputDeviceTrackerTest::ignoreSyntheticMouseMove() {
    InputDeviceTracker inputDeviceTracker;
    inputDeviceTracker.onKeyPress(InputKey::GAMEPAD_BUTTON_SOUTH);

    inputDeviceTracker.onMouseMove(0.0, 0.0);

    AssertHelper::assertTrue(inputDeviceTracker.getLastInputDevice() == InputDevice::GAMEPAD);
}

void InputDeviceTrackerTest::ignoreUnknownKey() {
    InputDeviceTracker inputDeviceTracker;
    inputDeviceTracker.onKeyPress(InputKey::GAMEPAD_BUTTON_SOUTH);

    inputDeviceTracker.onKeyPress(InputKey::UNKNOWN_KEY);

    AssertHelper::assertTrue(inputDeviceTracker.getLastInputDevice() == InputDevice::GAMEPAD);
}

CppUnit::Test* InputDeviceTrackerTest::suite() {
    auto* suite = new CppUnit::TestSuite("InputDeviceTrackerTest");

    suite->addTest(new CppUnit::TestCaller("switchToGamepadByKey", &InputDeviceTrackerTest::switchToGamepadByKey));
    suite->addTest(new CppUnit::TestCaller("switchToGamepadByAnalogInput", &InputDeviceTrackerTest::switchToGamepadByAnalogInput));
    suite->addTest(new CppUnit::TestCaller("ignoreSmallAnalogInput", &InputDeviceTrackerTest::ignoreSmallAnalogInput));
    suite->addTest(new CppUnit::TestCaller("switchBackToKeyboardMouse", &InputDeviceTrackerTest::switchBackToKeyboardMouse));
    suite->addTest(new CppUnit::TestCaller("ignoreSyntheticMouseMove", &InputDeviceTrackerTest::ignoreSyntheticMouseMove));
    suite->addTest(new CppUnit::TestCaller("ignoreUnknownKey", &InputDeviceTrackerTest::ignoreUnknownKey));

    return suite;
}
