#include <limits>

#include "BodyAABBTree.h"
#include "collision/broadphase/VectorPairContainer.h"

namespace urchin {
    BodyAABBTree::BodyAABBTree() :
            AABBTree<AbstractWorkBody *>(ConfigService::instance()->getFloatValue("broadPhase.aabbTreeFatMargin")),
            defaultPairContainer(new VectorPairContainer()),
            inInitializationPhase(true),
            minYBoundary(std::numeric_limits<float>::max()) {

    }

    BodyAABBTree::~BodyAABBTree() {
        delete defaultPairContainer;
    }

    void BodyAABBTree::addBody(AbstractWorkBody *body, PairContainer *alternativePairContainer) {
        auto *nodeData = new BodyAABBNodeData(body, alternativePairContainer);
        AABBTree::addObject(nodeData);
    }

    void BodyAABBTree::postAddObjectCallback(AABBNode<AbstractWorkBody *> *newNode) {
        computeOverlappingPairsfor (newNode);
    }

    void BodyAABBTree::removeBody(AbstractWorkBody *body) {
        auto *nodeData = AABBTree::getNodeData(body);
        AABBTree::removeObject(nodeData);
    }

    void BodyAABBTree::preRemoveObjectCallback(AABBNode<AbstractWorkBody *> *nodeToDelete) {
        auto *bodyNodeToDelete = dynamic_cast<BodyAABBNodeData *>(nodeToDelete->getNodeData());
        removeOverlappingPairs(bodyNodeToDelete);
    }

    void BodyAABBTree::updateBodies() {
        if (inInitializationPhase) {
            computeWorldBoundary();
            inInitializationPhase = false;
        }

        AABBTree::updateObjects();
    }

    void BodyAABBTree::preUpdateObjectCallback(AABBNode<AbstractWorkBody *> *nodeToUpdate) {
        controlBoundaries(nodeToUpdate);
    }

    const std::vector<OverlappingPair *> &BodyAABBTree::getOverlappingPairs() const {
        return defaultPairContainer->getOverlappingPairs();
    }

    void BodyAABBTree::computeOverlappingPairsfor (AABBNode<AbstractWorkBody *> *leafNode) {
        browseNodes.clear();
        browseNodes.push_back(AABBTree::getRootNode());

        for (std::size_t i=0; i<browseNodes.size(); ++i) { //tree traversal: pre-order (iterative)
            const AABBNode<AbstractWorkBody *> *currentNode = browseNodes[i];

            if (leafNode!=currentNode && leafNode->getAABBox().collideWithAABBox(currentNode->getAABBox())) {
                if (currentNode->isLeaf()) {
                    createOverlappingPair(dynamic_cast<BodyAABBNodeData *>(leafNode->getNodeData()), dynamic_cast<BodyAABBNodeData *>(currentNode->getNodeData()));
                } else {
                    browseNodes.push_back(currentNode->getRightChild());
                    browseNodes.push_back(currentNode->getLeftChild());
                }
            }
        }
    }

    void BodyAABBTree::createOverlappingPair(BodyAABBNodeData *nodeData1, BodyAABBNodeData *nodeData2) {
        if (!nodeData1->hasAlternativePairContainer() && !nodeData2->hasAlternativePairContainer()) {
            defaultPairContainer->addOverlappingPair(nodeData1->getNodeObject(), nodeData2->getNodeObject());
        } else {
            if (nodeData1->hasAlternativePairContainer()) {
                nodeData1->getAlternativePairContainer()->addOverlappingPair(nodeData1->getNodeObject(), nodeData2->getNodeObject());
                nodeData2->addOwnerPairContainer(nodeData1->getAlternativePairContainer());
            }

            if (nodeData2->hasAlternativePairContainer()) {
                nodeData2->getAlternativePairContainer()->addOverlappingPair(nodeData1->getNodeObject(), nodeData2->getNodeObject());
                nodeData1->addOwnerPairContainer(nodeData2->getAlternativePairContainer());
            }
        }
    }

    void BodyAABBTree::removeOverlappingPairs(const BodyAABBNodeData *nodeData) {
        if (!nodeData->hasAlternativePairContainer()) {
            defaultPairContainer->removeOverlappingPairs(nodeData->getNodeObject());
        } else {
            PairContainer *alternativePairContainer = nodeData->getAlternativePairContainer();

            removeAlternativePairContainerReferences(nodeData->getNodeObject(), alternativePairContainer);
            alternativePairContainer->removeOverlappingPairs(nodeData->getNodeObject());
        }

        for (auto &ownerPairContainer : nodeData->getOwnerPairContainers()) {
            ownerPairContainer->removeOverlappingPairs(nodeData->getNodeObject());
        }
    }

    void BodyAABBTree::removeAlternativePairContainerReferences(const AbstractWorkBody *body, PairContainer *alternativePairContainer) {
        std::vector<OverlappingPair> overlappingPairs = alternativePairContainer->retrieveCopyOverlappingPairs();
        for (const auto &overlappingPair : overlappingPairs) {
            AbstractWorkBody *otherPairBody = overlappingPair.getBody1() == body ? overlappingPair.getBody2() : overlappingPair.getBody1();
            auto *otherNodeData = dynamic_cast<BodyAABBNodeData *>(BodyAABBTree::getNodeData(otherPairBody));

            otherNodeData->removeOwnerPairContainer(alternativePairContainer);
        }
    }

    void BodyAABBTree::computeWorldBoundary() {
        float maxYBoundary = -std::numeric_limits<float>::max();
        for (auto &objectNode : objectsNode) {
            const AABBox<float> &nodeAABBox = objectNode.second->getAABBox();
            minYBoundary = std::min(nodeAABBox.getMin().Y, minYBoundary);
            maxYBoundary = std::max(nodeAABBox.getMax().Y, maxYBoundary);
        }

        float worldHeight = maxYBoundary - minYBoundary;
        minYBoundary -= worldHeight * BOUNDARIES_MARGIN_PERCENTAGE;
    }

    void BodyAABBTree::controlBoundaries(AABBNode<AbstractWorkBody *> *leafNode) {
        const AABBox<float> &bodyAABBox = leafNode->getNodeData()->retrieveObjectAABBox();

        if (bodyAABBox.getMax().Y < minYBoundary) {
            AbstractWorkBody *body = leafNode->getNodeData()->getNodeObject();

            std::stringstream logStream;
            logStream<<"Body "<<body->getId()<<" is below the limit of "<<std::to_string(minYBoundary)<<": "<<body->getPosition();
            Logger::logger().log(Logger::CriticalityLevel::WARNING, logStream.str());

            body->setIsStatic(true);
            body->setPosition(Point3<float>(body->getPosition().X, minYBoundary + bodyAABBox.getHalfSizes().Y + 0.01f, body->getPosition().Z));
        }
    }
}
