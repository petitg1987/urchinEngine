#include <algorithm>

#include "body/BodyManager.h"

namespace urchin {

    BodyManager::BodyManager() :
        lastUpdatedWorkBody(nullptr) {

    }

    BodyManager::~BodyManager() {
        for (auto &body : bodies) {
            delete body;
        }

        for (auto &workBody : workBodies) {
            delete workBody;
        }
    }

    void BodyManager::addBody(AbstractBody *body) {
        body->setIsNew(true);

        std::lock_guard<std::mutex> lock(bodiesMutex);
        this->bodies.push_back(body);
    }

    void BodyManager::removeBody(AbstractBody *body) {
        body->markAsDeleted();
    }

    AbstractWorkBody *BodyManager::getLastUpdatedWorkBody() const {
        return lastUpdatedWorkBody;
    }

    const std::vector<AbstractWorkBody *> &BodyManager::getWorkBodies() const {
        return workBodies;
    }

    /**
     * Setup work bodies with new data on bodies
     */
    void BodyManager::setupWorkBodies() {
        ScopeProfiler profiler("physics", "setupWorkBodies");
        std::lock_guard<std::mutex> lock(bodiesMutex);

        auto it = bodies.begin();
        while (it!=bodies.end()) {
            AbstractBody *body = *it;

            if (body->isNew()) {
                createNewWorkBody(body);
                ++it;
            } else if (body->isDeleted()) {
                it = deleteBody(body, it);
            } else if (body->needFullRefresh()) {
                deleteWorkBody(body);
                createNewWorkBody(body);
                ++it;
            } else {
                body->updateTo(body->getWorkBody());
                ++it;
            }
        }
    }

    void BodyManager::createNewWorkBody(AbstractBody *body) {
        //create new work body
        AbstractWorkBody *workBody = body->createWorkBody();
        body->setWorkBody(workBody);
        body->setIsNew(false);
        body->setNeedFullRefresh(false);
        workBodies.push_back(workBody);

        //update work body
        body->updateTo(workBody);

        //add notification
        lastUpdatedWorkBody = workBody;
        notifyObservers(this, ADD_WORK_BODY);
    }

    std::vector<AbstractBody *>::iterator BodyManager::deleteBody(AbstractBody *body, const std::vector<AbstractBody *>::iterator &it) {
        //delete work body
        deleteWorkBody(body);

        //delete body
        auto newIt = bodies.erase(it);
        delete body;

        return newIt;
    }

    void BodyManager::deleteWorkBody(AbstractBody *body) {
        AbstractWorkBody *workBody = body->getWorkBody();

        auto itFind = std::find(workBodies.begin(), workBodies.end(), workBody);
        if (itFind!=workBodies.end()) {
            //remove notification
            lastUpdatedWorkBody = workBody;
            notifyObservers(this, REMOVE_WORK_BODY);

            //delete reference
            body->setWorkBody(nullptr);

            //delete work body
            delete workBody;
            workBodies.erase(itFind);
        }
    }

    void BodyManager::applyWorkBodies() {
        std::lock_guard<std::mutex> lock(bodiesMutex);

        for (auto &body : bodies) {
            AbstractWorkBody *workBody = body->getWorkBody();
            if (!workBody) { //work body is not created yet
                continue;
            }

            body->applyFrom(workBody);
        }
    }

}
