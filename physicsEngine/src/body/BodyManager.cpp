#include <algorithm>

#include <body/BodyManager.h>

namespace urchin {

    BodyManager::BodyManager() :
        lastUpdatedBody(nullptr) {

    }

    BodyManager::~BodyManager() {
        for (auto& body : bodies) {
            delete body;
        }

        for (auto& bodyToRefresh : bodiesToRefresh) {
            if (bodyToRefresh.updateType == BodyRefresh::ADD) {
                delete bodyToRefresh.body;
            }
        }
    }

    void BodyManager::addBody(AbstractBody* body) {
        std::lock_guard<std::mutex> lock(bodiesMutex);

        BodyRefresh bodyUpdate{body, BodyRefresh::ADD};
        bodiesToRefresh.emplace_back(bodyUpdate);
    }

    void BodyManager::removeBody(AbstractBody* body) {
        std::lock_guard<std::mutex> lock(bodiesMutex);

        BodyRefresh bodyUpdate{body, BodyRefresh::REMOVE};
        bodiesToRefresh.emplace_back(bodyUpdate);
    }

    AbstractBody* BodyManager::getLastUpdatedBody() const {
        return lastUpdatedBody;
    }

    const std::vector<AbstractBody*>& BodyManager::getBodies() const {
        return bodies;
    }

    /**
     * Refresh bodies list
     */
    void BodyManager::refreshBodies() {
        ScopeProfiler sp(Profiler::physics(), "setupWorkBodies");
        std::lock_guard<std::mutex> lock(bodiesMutex);

        for (const auto& bodyToRefresh: bodiesToRefresh) {
            if (bodyToRefresh.updateType == BodyRefresh::ADD) {
                Logger::instance()->logInfo("Add new physical body: " + bodyToRefresh.body->getId());

                bodies.emplace_back(bodyToRefresh.body);
                bodyToRefresh.body->setPhysicsThreadId(std::this_thread::get_id());

                lastUpdatedBody = bodyToRefresh.body;
                notifyObservers(this, ADD_BODY);
            } else if (bodyToRefresh.updateType == BodyRefresh::REMOVE) {
                auto itFind = std::find(bodies.begin(), bodies.end(), bodyToRefresh.body);
                if (itFind != bodies.end()) {
                    bodies.erase(itFind);

                    lastUpdatedBody = bodyToRefresh.body;
                    notifyObservers(this, REMOVE_BODY);

                    delete bodyToRefresh.body;
                }
            }
        }
        bodiesToRefresh.clear();
    }
}
