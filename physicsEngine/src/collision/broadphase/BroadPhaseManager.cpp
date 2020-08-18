#include "collision/broadphase/BroadPhaseManager.h"
#include "collision/broadphase/aabbtree/AABBTreeAlgorithm.h"

namespace urchin
{

    BroadPhaseManager::BroadPhaseManager(BodyManager *bodyManager)
    {
        broadPhaseAlgorithm = new AABBTreeAlgorithm();

        bodyManager->addObserver(this, BodyManager::ADD_WORK_BODY);
        bodyManager->addObserver(this, BodyManager::REMOVE_WORK_BODY);
    }

    BroadPhaseManager::~BroadPhaseManager()
    {
        synchronizeBodies();
        delete broadPhaseAlgorithm;
    }

    void BroadPhaseManager::notify(Observable *observable, int notificationType)
    {
        if(auto *bodyManager = dynamic_cast<BodyManager *>(observable))
        {
            if(notificationType==BodyManager::ADD_WORK_BODY)
            {
                addBody(bodyManager->getLastUpdatedWorkBody());
            }else if(notificationType==BodyManager::REMOVE_WORK_BODY)
            {
                removeBody(bodyManager->getLastUpdatedWorkBody());
            }
        }
    }

    /**
     * Add body to broad phase. Method can be called from thread different of the physics thread.
     */
    void BroadPhaseManager::addBodyAsync(AbstractWorkBody *body)
    {
        std::lock_guard<std::mutex> lock(mutex);

        bodiesToAdd.push_back(body);
    }

    void BroadPhaseManager::addBody(AbstractWorkBody *body)
    {
        broadPhaseAlgorithm->addBody(body, body->getPairContainer());
    }

    /**
     * Remove body from broad phase. Method can be called from thread different of the physics thread.
     */
    void BroadPhaseManager::removeBodyAsync(AbstractWorkBody *body)
    {
        std::lock_guard<std::mutex> lock(mutex);

        bodiesToRemove.push_back(body);
    }

    void BroadPhaseManager::removeBody(AbstractWorkBody *body)
    {
        broadPhaseAlgorithm->removeBody(body);
    }

    void BroadPhaseManager::synchronizeBodies()
    {
        std::lock_guard<std::mutex> lock(mutex);

        for(auto &bodyToAdd : bodiesToAdd)
        {
            addBody(bodyToAdd);
        }
        bodiesToAdd.clear();

        for(auto &bodyToRemove : bodiesToRemove)
        {
            removeBody(bodyToRemove);
            delete bodyToRemove;
        }
        bodiesToRemove.clear();
    }

    const std::vector<OverlappingPair *> &BroadPhaseManager::computeOverlappingPairs()
    {
        ScopeProfiler profiler("physics", "coOverlapPair");

        synchronizeBodies();

        broadPhaseAlgorithm->updateBodies();
        return broadPhaseAlgorithm->getOverlappingPairs();
    }

    std::vector<AbstractWorkBody *> BroadPhaseManager::rayTest(const Ray<float> &ray) const
    {
        return broadPhaseAlgorithm->rayTest(ray);
    }

    std::vector<AbstractWorkBody *> BroadPhaseManager::bodyTest(AbstractWorkBody *body, const PhysicsTransform &from, const PhysicsTransform &to) const
    {
        return broadPhaseAlgorithm->bodyTest(body, from, to);
    }

}
