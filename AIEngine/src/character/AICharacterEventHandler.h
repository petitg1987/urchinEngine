#ifndef URCHINENGINE_AICHARACTEREVENTHANDLER_H
#define URCHINENGINE_AICHARACTEREVENTHANDLER_H

namespace urchin
{

    class AICharacterEventHandler
    {
        public:
            virtual ~AICharacterEventHandler() = default;

            virtual void startMoving();
            virtual void stopMoving();
    };

}

#endif
