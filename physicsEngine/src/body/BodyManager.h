#ifndef URCHINENGINE_BODYMANAGER_H
#define URCHINENGINE_BODYMANAGER_H

#include <mutex>

#include "body/model/AbstractBody.h"

namespace urchin {

    /**
    * A bodies manager allowing to manage bodies modifications coming from two different thread. Indeed, the user
    * can add/remove/update bodies from thread 1 while physics engine update the same bodies on thread 2.
    */
    class BodyManager : public Observable {
        public:
            BodyManager();
            ~BodyManager() override;

            enum NotificationType {
                ADD_BODY, //A body has been added to the world
                REMOVE_BODY, //A body has been removed from the world
            };

            void addBody(AbstractBody*);
            void removeBody(AbstractBody*);
            AbstractBody* getLastUpdatedBody() const;

            void refreshBodies();

            const std::vector<AbstractBody*>& getBodies() const;

        private:
            std::vector<AbstractBody*> bodies;

            std::vector<AbstractBody*> newBodies;
            std::vector<AbstractBody*> bodiesToDelete;

            mutable std::mutex bodiesMutex;

            AbstractBody* lastUpdatedBody;
    };

}

#endif
