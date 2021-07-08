#include <collision/broadphase/aabbtree/BodyAABBNodeData.h>

namespace urchin {

    BodyAABBNodeData::BodyAABBNodeData(AbstractBody* body) :
            AABBNodeData(body),
            bodyPairContainer(body->getPairContainer()) {

    }

    AABBNodeData<AbstractBody*>* BodyAABBNodeData::clone() const {
        return new BodyAABBNodeData(getNodeObject());
    }

    const std::string& BodyAABBNodeData::getObjectId() const {
        return getNodeObject()->getId();
    }

    AABBox<float> BodyAABBNodeData::retrieveObjectAABBox() const {
        return getNodeObject()->getShape().toAABBox(getNodeObject()->getTransform());
    }

    bool BodyAABBNodeData::isObjectMoving() const {
        return getNodeObject()->isActive();
    }

    bool BodyAABBNodeData::isGhostBody() const {
        return getNodeObject()->isGhostBody();
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
