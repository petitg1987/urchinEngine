#include <collision/bodystate/BodyActiveStateUpdater.h>

namespace urchin {

    //debug parameters
    constexpr auto DEBUG_PRINT_ISLANDS = False();

    BodyActiveStateUpdater::BodyActiveStateUpdater(const BodyContainer& bodyContainer) :
            bodyContainer(bodyContainer),
            squaredLinearSleepingThreshold(ConfigService::instance().getFloatValue("island.linearSleepingThreshold") * ConfigService::instance().getFloatValue("island.linearSleepingThreshold")),
            squaredAngularSleepingThreshold(ConfigService::instance().getFloatValue("island.angularSleepingThreshold") * ConfigService::instance().getFloatValue("island.angularSleepingThreshold")) {

    }

    /**
     * Refresh body active state. If all bodies of an island can sleep, we set their status to inactive.
     * If one body of the island cannot sleep, we set their status to active.
     * @param overlappingPairs Overlapping pairs of broad phase used to determine the islands
     */
    void BodyActiveStateUpdater::update(const std::vector<ManifoldResult>& manifoldResults) {
        ScopeProfiler sp(Profiler::physics(), "refreshBodyStat");

        buildIslands(manifoldResults);
        const std::vector<IslandElementLink>& islandElementsLink = islandContainer.retrieveSortedIslandElements();

        if (DEBUG_PRINT_ISLANDS) {
            printIslands(islandElementsLink);
        }

        unsigned int i = 0;
        while (islandElementsLink.size()>i) { //loop on islands
            unsigned int startElementIndex = i;
            unsigned int nbElements = computeNumberElements(islandElementsLink, startElementIndex);

            bool islandLinkedToStaticElement = false;
            bool islandBodiesCanSleep = true;
            for (unsigned int j = 0; j < nbElements; ++j) { //loop on elements of the island
                const auto* body = static_cast<RigidBody*>(islandElementsLink[startElementIndex + j].element);
                if (isBodyMoving(body)) {
                    islandBodiesCanSleep = false;
                    break;
                }

                islandLinkedToStaticElement = islandLinkedToStaticElement || islandElementsLink[startElementIndex + j].linkedToStaticElement;
            }
            islandBodiesCanSleep = islandBodiesCanSleep && islandLinkedToStaticElement; //one element of the island must be in contact with a static element to sleep the island

            for (unsigned int j = 0; j < nbElements; ++j) { //loop on elements of the island
                auto* body = static_cast<RigidBody*>(islandElementsLink[startElementIndex + j].element);
                bool bodyActiveState = !islandBodiesCanSleep;
                if (body->isActive() != bodyActiveState) {
                    body->setIsActive(bodyActiveState);

                    if (bodyActiveState) {
                        body->setVelocity(Vector3<float>(0.0f, 0.0f, 0.0f), Vector3<float>(0.0f, 0.0f, 0.0f));
                    }
                }
            }

            i += nbElements;
        }
    }

    void BodyActiveStateUpdater::buildIslands(const std::vector<ManifoldResult>& manifoldResults) {
        //1. create an island for each body
        islandElements.clear();
        for (auto& body : bodyContainer.getBodies()) {
            if (!body->isStatic()) {
                islandElements.push_back(body.get());
            }
        }
        islandContainer.reset(islandElements);

        //2. merge islands for bodies in contact
        for (const auto& manifoldResult : manifoldResults) {
            if (manifoldResult.getNumContactPoints() > 0) {
                const AbstractBody& body1 = manifoldResult.getBody1();
                const AbstractBody& body2 = manifoldResult.getBody2();

                if (!body1.isStatic() && !body2.isStatic()) {
                    islandContainer.mergeIsland(body1, body2);
                } else if (!body1.isStatic() && body2.isStatic()) {
                    islandContainer.linkToStaticElement(body1);
                } else if (!body2.isStatic() && body1.isStatic()) {
                    islandContainer.linkToStaticElement(body2);
                }
            }
        }
    }

    /**
     * @return Number of element for island starting at 'startElementIndex'
     */
    unsigned int BodyActiveStateUpdater::computeNumberElements(const std::vector<IslandElementLink>& islandElementsLink, unsigned int startElementIndex) const {
        unsigned int islandId = islandElementsLink[startElementIndex].islandIdRef;
        unsigned int endElementIndex;

        for (endElementIndex = startElementIndex; islandElementsLink.size() > endElementIndex && islandId == islandElementsLink[endElementIndex].islandIdRef; ++endElementIndex) {
            //nothing to do
        }

        return endElementIndex - startElementIndex;
    }

    bool BodyActiveStateUpdater::isBodyMoving(const RigidBody* body) const {
        return !(body->getLinearVelocity().squareLength() < squaredLinearSleepingThreshold
                 && body->getAngularVelocity().squareLength() < squaredAngularSleepingThreshold);
    }

    void BodyActiveStateUpdater::printIslands(const std::vector<IslandElementLink>& islandElementsLink) const {
        unsigned int islandId = 0;
        unsigned int i = 0;
        while (islandElementsLink.size()>i) { //loop on islands
            unsigned int startElementIndex = i;
            unsigned int nbElements = computeNumberElements(islandElementsLink, startElementIndex);

            std::cout << "Island " << islandId << ":" << std::endl;

            for (unsigned int j = 0; j < nbElements; ++j) { //loop on elements of the island
                const auto* body = static_cast<RigidBody*>(islandElementsLink[startElementIndex + j].element);
                std::cout << "  - Body: " << body->getId() << " (moving: " << isBodyMoving(body) << ", active: " << body->isActive() << ")" << std::endl;
            }

            i += nbElements;
            islandId++;
        }

        std::cout << std::endl;
    }

}
