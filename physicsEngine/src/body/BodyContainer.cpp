#include <algorithm>
#include <utility>

#include <body/BodyContainer.h>

namespace urchin {

    BodyContainer::BodyContainer() :
            lastUpdatedBody(nullptr) {

    }

    void BodyContainer::addBody(std::shared_ptr<AbstractBody> body) {
        std::lock_guard<std::mutex> lock(bodiesMutex);

        BodyRefresh bodyUpdate{nullptr, std::move(body)};
        bodiesToRefresh.emplace_back(bodyUpdate);
    }

    void BodyContainer::removeBody(const AbstractBody& body) {
        std::lock_guard<std::mutex> lock(bodiesMutex);

        BodyRefresh bodyUpdate{&body, std::shared_ptr<AbstractBody>(nullptr)};
        bodiesToRefresh.emplace_back(bodyUpdate);
    }

    AbstractBody* BodyContainer::getLastUpdatedBody() const {
        return lastUpdatedBody;
    }

    const std::vector<std::shared_ptr<AbstractBody>>& BodyContainer::getBodies() const {
        return bodies;
    }

    /**
     * Refresh bodies list
     */
    void BodyContainer::refreshBodies() {
        ScopeProfiler sp(Profiler::physics(), "setupWorkBodies");
        std::lock_guard<std::mutex> lock(bodiesMutex);

        for (const auto& bodyToRefresh: bodiesToRefresh) {
            if (bodyToRefresh.bodyToAdd) {
                bodies.emplace_back(bodyToRefresh.bodyToAdd);
                bodyToRefresh.bodyToAdd->setPhysicsThreadId(std::this_thread::get_id());

                lastUpdatedBody = bodyToRefresh.bodyToAdd.get();
                notifyObservers(this, ADD_BODY);
            }

            if (bodyToRefresh.bodyToRemove) {
                auto itFind = std::find_if(bodies.begin(), bodies.end(), [&bodyToRefresh](const auto& o){return o.get() == bodyToRefresh.bodyToRemove;});
                if (itFind != bodies.end()) {
                    std::shared_ptr<AbstractBody> bodyToRemovePtr = *itFind; //keep a smart pointer on body for notify event
                    bodies.erase(itFind);

                    lastUpdatedBody = bodyToRemovePtr.get();
                    notifyObservers(this, REMOVE_BODY);
                }
            }
        }
        bodiesToRefresh.clear();
    }
}
