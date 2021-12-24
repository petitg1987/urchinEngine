#include <character/CharacterEventCallback.h>

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
        std::cout<<"start walking"<<std::endl; //TODO rem
        //can be overridden
    }

    void CharacterEventCallback::onStopWalking() {
        std::cout<<"stop walking"<<std::endl; //TODO rem
        //can be overridden
    }

    void CharacterEventCallback::onStartRunning() {
        std::cout<<"start running"<<std::endl; //TODO rem
        //can be overridden
    }

    void CharacterEventCallback::onStopRunning() {
        std::cout<<"stop running"<<std::endl; //TODO rem
        //can be overridden
    }

    void CharacterEventCallback::onHit(float hitSpeed) {
        std::cout<<"hit ground: "<<hitSpeed<<std::endl; //TODO rem
        //can be overridden
    }

    void CharacterEventCallback::onStartJumping() {
        std::cout<<"jump"<<std::endl; //TODO rem
        //can be overridden
    }

}
