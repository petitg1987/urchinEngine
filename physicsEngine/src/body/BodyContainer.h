#pragma once

#include <mutex>

#include "body/model/AbstractBody.h"

namespace urchin {

    struct BodyRefresh {
        const AbstractBody* bodyToRemove;
        std::shared_ptr<AbstractBody> bodyToAdd;
    };

    /**
    * A bodies manager allowing to manage bodies modifications coming from two different thread. Indeed, the user
    * can add/remove/update bodies from thread 1 while physics engine update the same bodies on thread 2.
    */
    class BodyContainer final : public Observable {
        public:
            BodyContainer();

            enum NotificationType {
                ADD_BODY, //A body has been added to the world
                REMOVE_BODY, //A body has been removed from the world
            };

            void addBody(std::shared_ptr<AbstractBody>);
            void removeBody(const AbstractBody&);
            const std::shared_ptr<AbstractBody>& getLastUpdatedBody() const;

            void refreshBodies();

            const std::vector<std::shared_ptr<AbstractBody>>& getBodies() const;

        private:
            mutable std::mutex bodiesMutex;
            std::vector<std::shared_ptr<AbstractBody>> bodies;
            std::vector<BodyRefresh> bodiesToRefresh;

            std::shared_ptr<AbstractBody> lastUpdatedBody;
    };

}
