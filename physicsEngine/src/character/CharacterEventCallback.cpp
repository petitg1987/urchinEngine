#include "character/CharacterEventCallback.h"

namespace urchin {

    CharacterEventCallback::CharacterEventCallback() :
            isWalking(false),
            isRunning(false) {

    }

    void CharacterEventCallback::notifyWalking() {
        if (!isWalking) {
            notifyNoStepping();
            isWalking = true;
            onStartWalking();
        }
    }

    void CharacterEventCallback::notifyRunning() {
        if (!isRunning) {
            notifyNoStepping();
            isRunning = true;
            onStartRunning();
        }
    }

    void CharacterEventCallback::notifyNoStepping() {
        if (isWalking) {
            isWalking = false;
            onStopWalking();
        } else if (isRunning) {
            isRunning = false;
            onStopRunning();
        }
    }

    void CharacterEventCallback::onStartWalking() {
        //can be overridden
    }

    void CharacterEventCallback::onStopWalking() {
        //can be overridden
    }

    void CharacterEventCallback::onStartRunning() {
        //can be overridden
    }

    void CharacterEventCallback::onStopRunning() {
        //can be overridden
    }

    void CharacterEventCallback::onHitGround(float) {
        //can be overridden
    }

    void CharacterEventCallback::onStartJumping() {
        //can be overridden
    }

}
