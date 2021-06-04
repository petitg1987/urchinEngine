#pragma once

namespace urchin {

    class AICharacterEventHandler {
        public:
            virtual ~AICharacterEventHandler() = default;

            virtual void startMoving();
            virtual void stopMoving();
    };

}
