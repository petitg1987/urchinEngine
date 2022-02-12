#include <limits>

#include <collision/broadphase/aabbtree/BodyAABBTree.h>
#include <collision/broadphase/VectorPairContainer.h>

namespace urchin {

    BodyAABBTree::BodyAABBTree() :
            AABBTree<std::shared_ptr<AbstractBody>>(ConfigService::instance().getFloatValue("broadPhase.aabbTreeFatMargin")),
            defaultPairContainer(VectorPairContainer()),
            inInitializationPhase(true),
            minYBoundary(std::numeric_limits<float>::max()) {

    }

    void BodyAABBTree::addBody(const std::shared_ptr<AbstractBody>& body) {
        auto nodeData = std::make_unique<BodyAABBNodeData>(body);
        AABBTree::addObject(std::move(nodeData));
    }

    void BodyAABBTree::postAddObjectCallback(AABBNode<std::shared_ptr<AbstractBody>>& newNode) {
        computeOverlappingPairs(newNode);
    }

    void BodyAABBTree::removeBody(const AbstractBody& body) {
        auto* bodyPtr = const_cast<AbstractBody*>(&body);
        auto& nodeData = AABBTree::getNodeData(bodyPtr);
        AABBTree::removeObject(nodeData);
    }

    void BodyAABBTree::preRemoveObjectCallback(AABBNode<std::shared_ptr<AbstractBody>>& nodeToDelete) {
        const auto& bodyNodeToDelete = static_cast<BodyAABBNodeData&>(nodeToDelete.getNodeData());
        removeOverlappingPairs(bodyNodeToDelete);
    }

    void BodyAABBTree::updateBodies() {
        if (inInitializationPhase) {
            computeWorldBoundary();
            inInitializationPhase = false;
        }

        AABBTree::updateObjects();
    }

    void BodyAABBTree::preUpdateObjectCallback(AABBNode<std::shared_ptr<AbstractBody>>& nodeToUpdate) {
        controlBoundaries(nodeToUpdate);
    }

    const std::vector<std::unique_ptr<OverlappingPair>>& BodyAABBTree::getOverlappingPairs() const {
        return defaultPairContainer.getOverlappingPairs();
    }

    void BodyAABBTree::computeOverlappingPairs(const AABBNode<std::shared_ptr<AbstractBody>>& leafNode) {
        browseNodes.clear();
        browseNodes.push_back(AABBTree::getRootNode());

        for (std::size_t i = 0; i < browseNodes.size(); ++i) { //tree traversal: pre-order (iterative)
            const AABBNode<std::shared_ptr<AbstractBody>>* currentNode = browseNodes[i];

            if (&leafNode != currentNode && leafNode.getAABBox().collideWithAABBox(currentNode->getAABBox())) {
                if (currentNode->isLeaf()) {
                    createOverlappingPair(static_cast<BodyAABBNodeData&>(leafNode.getNodeData()), static_cast<BodyAABBNodeData&>(currentNode->getNodeData()));
                } else {
                    browseNodes.push_back(currentNode->getRightChild());
                    browseNodes.push_back(currentNode->getLeftChild());
                }
            }
        }
    }

    void BodyAABBTree::createOverlappingPair(BodyAABBNodeData& nodeData1, BodyAABBNodeData& nodeData2) {
        if (!nodeData1.isGhostBody() && !nodeData2.isGhostBody()) {
            defaultPairContainer.addOverlappingPair(nodeData1.getNodeObject(), nodeData2.getNodeObject());
        } else if (nodeData1.isGhostBody() && !nodeData2.isGhostBody()) {
            nodeData1.getBodyPairContainer()->addOverlappingPair(nodeData1.getNodeObject(), nodeData2.getNodeObject());
            nodeData2.addOwnerPairContainer(nodeData1.getBodyPairContainer());
        } else if (nodeData2.isGhostBody() && !nodeData1.isGhostBody()) {
            nodeData2.getBodyPairContainer()->addOverlappingPair(nodeData1.getNodeObject(), nodeData2.getNodeObject());
            nodeData1.addOwnerPairContainer(nodeData2.getBodyPairContainer());
        } else {
            //ghost bodies cannot see each other
        }
    }

    void BodyAABBTree::removeOverlappingPairs(const BodyAABBNodeData& nodeData) {
        if (!nodeData.isGhostBody()) {
            defaultPairContainer.removeOverlappingPairs(*nodeData.getNodeObject());
        } else {
            removeBodyPairContainerReferences(*nodeData.getNodeObject(), nodeData.getBodyPairContainer());
            nodeData.getBodyPairContainer()->removeOverlappingPairs(*nodeData.getNodeObject());
        }

        for (auto& ownerPairContainer : nodeData.getOwnerPairContainers()) {
            ownerPairContainer->removeOverlappingPairs(*nodeData.getNodeObject());
        }
    }

    void BodyAABBTree::removeBodyPairContainerReferences(const AbstractBody& body, PairContainer* bodyPairContainer) {
        std::vector<OverlappingPair> overlappingPairs;
        bodyPairContainer->retrieveCopyOverlappingPairs(overlappingPairs);

        for (const auto& overlappingPair : overlappingPairs) {
            const std::shared_ptr<AbstractBody>& otherPairBody = &overlappingPair.getBody1() == &body ? overlappingPair.getBody2Ptr() : overlappingPair.getBody1Ptr();
            auto& otherNodeData = static_cast<BodyAABBNodeData&>(AABBTree::getNodeData(otherPairBody.get()));

            otherNodeData.removeOwnerPairContainer(bodyPairContainer);
        }
    }

    void BodyAABBTree::computeWorldBoundary() {
        float maxYBoundary = -std::numeric_limits<float>::max();
        for (const auto& [objPtr, objNode] : objectsNode) {
            const AABBox<float>& nodeAABBox = objNode->getAABBox();
            minYBoundary = std::min(nodeAABBox.getMin().Y, minYBoundary);
            maxYBoundary = std::max(nodeAABBox.getMax().Y, maxYBoundary);
        }

        float worldHeight = maxYBoundary - minYBoundary;
        minYBoundary -= worldHeight * BOUNDARIES_MARGIN_PERCENTAGE;
    }

    void BodyAABBTree::controlBoundaries(const AABBNode<std::shared_ptr<AbstractBody>>& leafNode) const {
        const AABBox<float>& bodyAABBox = leafNode.getNodeData().retrieveObjectAABBox();

        if (bodyAABBox.getMax().Y < minYBoundary) {
            std::shared_ptr<AbstractBody> body = leafNode.getNodeData().getNodeObject();
            if (!body->isStatic()) {
                Logger::instance().logWarning("Body " + body->getId() + " is below the limit of " + TypeConverter::toString(minYBoundary) + ": " + StringUtil::toString(body->getTransform().getPosition()));
                body->setIsActive(false);
            }
        }
    }
}
