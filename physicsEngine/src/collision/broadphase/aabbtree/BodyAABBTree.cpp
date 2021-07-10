#include <limits>

#include <collision/broadphase/aabbtree/BodyAABBTree.h>
#include <collision/broadphase/VectorPairContainer.h>

namespace urchin {

    //static
    constexpr float BodyAABBTree::BOUNDARIES_MARGIN_PERCENTAGE = 0.3f;

    BodyAABBTree::BodyAABBTree() :
            AABBTree<AbstractBody*>(ConfigService::instance()->getFloatValue("broadPhase.aabbTreeFatMargin")),
            defaultPairContainer(std::make_unique<VectorPairContainer>()),
            inInitializationPhase(true),
            minYBoundary(std::numeric_limits<float>::max()) {

    }

    void BodyAABBTree::addBody(AbstractBody* body) {
        auto* nodeData = new BodyAABBNodeData(body);
        AABBTree::addObject(nodeData);
    }

    void BodyAABBTree::postAddObjectCallback(AABBNode<AbstractBody*>* newNode) {
        computeOverlappingPairsFor(newNode);
    }

    void BodyAABBTree::removeBody(AbstractBody* body) {
        auto* nodeData = AABBTree::getNodeData(body);
        AABBTree::removeObject(nodeData);
    }

    void BodyAABBTree::preRemoveObjectCallback(AABBNode<AbstractBody*>* nodeToDelete) {
        auto* bodyNodeToDelete = dynamic_cast<BodyAABBNodeData*>(nodeToDelete->getNodeData());
        removeOverlappingPairs(bodyNodeToDelete);
    }

    void BodyAABBTree::updateBodies() {
        if (inInitializationPhase) {
            computeWorldBoundary();
            inInitializationPhase = false;
        }

        AABBTree::updateObjects();
    }

    void BodyAABBTree::preUpdateObjectCallback(AABBNode<AbstractBody*>* nodeToUpdate) {
        controlBoundaries(nodeToUpdate);
    }

    const std::vector<std::unique_ptr<OverlappingPair>>& BodyAABBTree::getOverlappingPairs() const {
        return defaultPairContainer->getOverlappingPairs();
    }

    void BodyAABBTree::computeOverlappingPairsFor(AABBNode<AbstractBody*>* leafNode) {
        browseNodes.clear();
        browseNodes.push_back(AABBTree::getRootNode());

        for (std::size_t i = 0; i < browseNodes.size(); ++i) { //tree traversal: pre-order (iterative)
            const AABBNode<AbstractBody*>* currentNode = browseNodes[i];

            if (leafNode != currentNode && leafNode->getAABBox().collideWithAABBox(currentNode->getAABBox())) {
                if (currentNode->isLeaf()) {
                    createOverlappingPair(dynamic_cast<BodyAABBNodeData*>(leafNode->getNodeData()), dynamic_cast<BodyAABBNodeData*>(currentNode->getNodeData()));
                } else {
                    browseNodes.push_back(currentNode->getRightChild());
                    browseNodes.push_back(currentNode->getLeftChild());
                }
            }
        }
    }

    void BodyAABBTree::createOverlappingPair(BodyAABBNodeData* nodeData1, BodyAABBNodeData* nodeData2) {
        if (!nodeData1->isGhostBody() && !nodeData2->isGhostBody()) {
            defaultPairContainer->addOverlappingPair(nodeData1->getNodeObject(), nodeData2->getNodeObject());
        } else if (nodeData1->isGhostBody() && !nodeData2->isGhostBody()) {
            nodeData1->getBodyPairContainer()->addOverlappingPair(nodeData1->getNodeObject(), nodeData2->getNodeObject());
            nodeData2->addOwnerPairContainer(nodeData1->getBodyPairContainer());
        } else if (nodeData2->isGhostBody() && !nodeData1->isGhostBody()) {
            nodeData2->getBodyPairContainer()->addOverlappingPair(nodeData1->getNodeObject(), nodeData2->getNodeObject());
            nodeData1->addOwnerPairContainer(nodeData2->getBodyPairContainer());
        } else {
            //ghost bodies cannot see each other
        }
    }

    void BodyAABBTree::removeOverlappingPairs(const BodyAABBNodeData* nodeData) {
        if (!nodeData->isGhostBody()) {
            defaultPairContainer->removeOverlappingPairs(nodeData->getNodeObject());
        } else {
            removeBodyPairContainerReferences(nodeData->getNodeObject(), nodeData->getBodyPairContainer());
            nodeData->getBodyPairContainer()->removeOverlappingPairs(nodeData->getNodeObject());
        }

        for (auto& ownerPairContainer : nodeData->getOwnerPairContainers()) {
            ownerPairContainer->removeOverlappingPairs(nodeData->getNodeObject());
        }
    }

    void BodyAABBTree::removeBodyPairContainerReferences(const AbstractBody* body, PairContainer* bodyPairContainer) {
        std::vector<OverlappingPair> overlappingPairs = bodyPairContainer->retrieveCopyOverlappingPairs();
        for (const auto& overlappingPair : overlappingPairs) {
            AbstractBody* otherPairBody = overlappingPair.getBody1() == body ? overlappingPair.getBody2() : overlappingPair.getBody1();
            auto* otherNodeData = dynamic_cast<BodyAABBNodeData*>(BodyAABBTree::getNodeData(otherPairBody));

            otherNodeData->removeOwnerPairContainer(bodyPairContainer);
        }
    }

    void BodyAABBTree::computeWorldBoundary() {
        float maxYBoundary = -std::numeric_limits<float>::max();
        for (auto& objectNode : objectsNode) {
            const AABBox<float>& nodeAABBox = objectNode.second->getAABBox();
            minYBoundary = std::min(nodeAABBox.getMin().Y, minYBoundary);
            maxYBoundary = std::max(nodeAABBox.getMax().Y, maxYBoundary);
        }

        float worldHeight = maxYBoundary - minYBoundary;
        minYBoundary -= worldHeight * BOUNDARIES_MARGIN_PERCENTAGE;
    }

    void BodyAABBTree::controlBoundaries(AABBNode<AbstractBody*>* leafNode) const {
        const AABBox<float>& bodyAABBox = leafNode->getNodeData()->retrieveObjectAABBox();

        if (bodyAABBox.getMax().Y < minYBoundary) {
            AbstractBody* body = leafNode->getNodeData()->getNodeObject();
            if (!body->isStatic()) {
                std::stringstream logStream;
                logStream << "Body " << body->getId() << " is below the limit of " << std::to_string(minYBoundary) << ": " << body->getTransform().getPosition();
                Logger::instance()->logWarning(logStream.str());

                body->setIsActive(false);
            }
        }
    }
}
