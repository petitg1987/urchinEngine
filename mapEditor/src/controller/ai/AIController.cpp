#include <memory>
#include <controller/ai/AIController.h>

#include <controller/ai/AIController.h>

namespace urchin {

    AIController::AIController() :
            AbstractController() {

    }

    const SceneAI& AIController::getSceneAI() const {
        return getMapHandler()->getMap()->getSceneAI();
    }

    const SceneAI& AIController::updateSceneAI(std::unique_ptr<NavMeshAgent> navMeshAgent) {
        getMapHandler()->getMap()->updateSceneAI(std::move(navMeshAgent));

        markModified();
        return getMapHandler()->getMap()->getSceneAI();
    }

}
