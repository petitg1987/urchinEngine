#include <collision/broadphase/aabbtree/BodyAABBNodeData.h>

namespace urchin {

    BodyAABBNodeData::BodyAABBNodeData(const std::shared_ptr<AbstractBody>& body) :
            AABBNodeData(body),
            bodyPairContainer(body->getPairContainer()) {

    }

    std::unique_ptr<AABBNodeData<std::shared_ptr<AbstractBody>>> BodyAABBNodeData::clone() const {
        return std::make_unique<BodyAABBNodeData>(getNodeObject());
    }

    const std::string& BodyAABBNodeData::getObjectId() const {
        return getNodeObject()->getId();
    }

    AABBox<float> BodyAABBNodeData::retrieveObjectAABBox() const {
        return getNodeObject()->getShape().toAABBox(getNodeObject()->getTransform());
    }

    bool BodyAABBNodeData::isObjectMoving() const {
        return getNodeObject()->isActive() || getNodeObject()->getManuallyMovedAndReset();
    }

    bool BodyAABBNodeData::isGhostBody() const {
        return getNodeObject()->getBodyType() == BodyType::GHOST;
    }

    PairContainer* BodyAABBNodeData::getBodyPairContainer() const {
        return bodyPairContainer;
    }

    void BodyAABBNodeData::addOwnerPairContainer(PairContainer* ownerPairContainer) {
        ownerPairContainers.insert(ownerPairContainer);
    }

    void BodyAABBNodeData::removeOwnerPairContainer(PairContainer* ownerPairContainer) {
        ownerPairContainers.erase(ownerPairContainer);
    }

    /**
     * Returns pair containers which have pair(s) with this node data
     */
    std::set<PairContainer*> BodyAABBNodeData::getOwnerPairContainers() const {
        return ownerPairContainers;
    }

}
