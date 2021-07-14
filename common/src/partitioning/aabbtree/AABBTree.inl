
template<class OBJ> AABBTree<OBJ>::AABBTree(float fatMargin) :
        fatMargin(fatMargin) {

}

template<class OBJ> void AABBTree<OBJ>::updateFatMargin(float fatMargin) {
    this->fatMargin = fatMargin;

    std::vector<std::unique_ptr<AABBNodeData<OBJ>>> allNodeData = extractAllNodeData();
    rootNode = nullptr;
    for (auto& nodeData : allNodeData) {
        addObject(std::move(nodeData));
    }
}

template <class OBJ> AABBNode<OBJ>* AABBTree<OBJ>::getRootNode() const {
    return rootNode.get();
}

template <class OBJ> AABBNodeData<OBJ>& AABBTree<OBJ>::getNodeData(OBJ object) const {
    return objectsNode.at(object)->getNodeData();
}

/**
 *
 * @tparam nodeObjects [out] Returns all node objects in the tree
 */
template <class OBJ> void AABBTree<OBJ>::getAllNodeObjects(std::vector<OBJ>& nodeObjects) const {
    browseNodes.clear();
    if (rootNode != nullptr) {
        browseNodes.push_back(rootNode.get());
    }

    for (std::size_t i = 0; i < browseNodes.size(); ++i) { //tree traversal: pre-order (iterative)
        const AABBNode<OBJ>* currentNode = browseNodes[i];

        if (currentNode->isLeaf()) {
            nodeObjects.push_back(currentNode->getNodeData().getNodeObject());
        } else {
            browseNodes.push_back(currentNode->getRightChild());
            browseNodes.push_back(currentNode->getLeftChild());
        }
    }
}

template <class OBJ> std::vector<std::unique_ptr<AABBNodeData<OBJ>>> AABBTree<OBJ>::extractAllNodeData() {
    std::vector<std::unique_ptr<AABBNodeData<OBJ>>> allNodeData = {};

    browseNodes.clear();
    if (rootNode != nullptr) {
        browseNodes.push_back(rootNode.get());
    }

    for (std::size_t i = 0; i < browseNodes.size(); ++i) { //tree traversal: pre-order (iterative)
        AABBNode<OBJ>* currentNode = browseNodes[i];

        if (currentNode->isLeaf()) {
            allNodeData.push_back(currentNode->moveNodeData());
        } else {
            browseNodes.push_back(currentNode->getRightChild());
            browseNodes.push_back(currentNode->getLeftChild());
        }
    }

    return allNodeData;
}

template <class OBJ> void AABBTree<OBJ>::addObject(std::unique_ptr<AABBNodeData<OBJ>> nodeData) {
    auto nodeToInsert = std::make_shared<AABBNode<OBJ>>(std::move(nodeData));

    if (rootNode) {
        nodeToInsert->updateAABBox(fatMargin);
        insertNode(nodeToInsert, rootNode);
    } else {
        rootNode = nodeToInsert;
        rootNode->updateAABBox(fatMargin);
    }

    objectsNode[nodeToInsert->getNodeData().getNodeObject()] = nodeToInsert;

    postAddObjectCallback(*nodeToInsert);
}

template<class OBJ> void AABBTree<OBJ>::postAddObjectCallback(AABBNode<OBJ>&) {
    //can be override
}

template<class OBJ> void AABBTree<OBJ>::insertNode(std::shared_ptr<AABBNode<OBJ>> nodeToInsert, std::shared_ptr<AABBNode<OBJ>> currentNode) {
    if (currentNode->isLeaf()) {
        auto newParent = std::make_shared<AABBNode<OBJ>>(nullptr);
        replaceNode(*currentNode, newParent);
        newParent->setLeftChild(std::move(nodeToInsert));
        newParent->setRightChild(currentNode);
        newParent->updateAABBox(fatMargin);
    } else {
        const AABBox<float>& leftAABBox = currentNode->getLeftChild()->getAABBox();
        float volumeDiffLeft = leftAABBox.merge(nodeToInsert->getAABBox()).getVolume() - leftAABBox.getVolume();

        const AABBox<float>& rightAABBox = currentNode->getRightChild()->getAABBox();
        float volumeDiffRight = rightAABBox.merge(nodeToInsert->getAABBox()).getVolume() - rightAABBox.getVolume();

        if (volumeDiffLeft < volumeDiffRight) {
            insertNode(std::move(nodeToInsert), currentNode->getLeftChildSmartPtr());
        } else {
            insertNode(std::move(nodeToInsert), currentNode->getRightChildSmartPtr());
        }

        currentNode->updateAABBox(fatMargin);
    }
}

template<class OBJ> void AABBTree<OBJ>::replaceNode(const AABBNode<OBJ>& nodeToReplace, std::shared_ptr<AABBNode<OBJ>> newNode) {
    if (nodeToReplace.getParent()) {
        if (nodeToReplace.getParent()->getLeftChild() == &nodeToReplace) {
            nodeToReplace.getParent()->setLeftChild(std::move(newNode));
        } else {
            nodeToReplace.getParent()->setRightChild(std::move(newNode));
        }
    } else {
        rootNode = std::move(newNode);
        rootNode->setParent(nullptr);
    }
}

template<class OBJ> void AABBTree<OBJ>::removeObject(AABBNodeData<OBJ>& nodeData) {
    removeObject(nodeData.getNodeObject());
}

template<class OBJ> void AABBTree<OBJ>::removeObject(OBJ object) {
    auto itFind = objectsNode.find(object);
    if (itFind != objectsNode.end()) {
        AABBNode<OBJ>* nodeToRemove = itFind->second.get();
        preRemoveObjectCallback(*nodeToRemove);

        objectsNode.erase(object);
        removeNode(nodeToRemove);
    }
}

template<class OBJ> void AABBTree<OBJ>::preRemoveObjectCallback(AABBNode<OBJ>&) {
    //can be override
}

template<class OBJ> void AABBTree<OBJ>::removeNode(AABBNode<OBJ>* nodeToRemove) {
    std::shared_ptr<AABBNode<OBJ>> parentNode = nodeToRemove->getParent();

    if (!parentNode) {
        rootNode = nullptr;
    } else {
        std::shared_ptr<AABBNode<OBJ>> sibling = nodeToRemove->getSibling();
        replaceNode(*parentNode, sibling);

        parentNode->setLeftChild(nullptr); //avoid child removal
        parentNode->setRightChild(nullptr); //avoid child removal
    }
}

template<class OBJ> void AABBTree<OBJ>::updateObjects() {
    for (int i = (int)objectsNode.size() - 1; i >= 0; --i) { //loop backward to remove elements
        auto it = objectsNode.begin();
        std::advance(it, i);

        std::shared_ptr<AABBNode<OBJ>> leaf = it->second;
        if (leaf->getNodeData().isObjectMoving()) {
            preUpdateObjectCallback(*leaf);

            const AABBox<float>& leafFatAABBox = leaf->getAABBox();
            const AABBox<float>& objectAABBox = leaf->getNodeData().retrieveObjectAABBox();

            if (!leafFatAABBox.include(objectAABBox)) {
                std::unique_ptr<AABBNodeData<OBJ>> clonedNodeData = leaf->getNodeData().clone();
                removeObject(leaf->getNodeData());
                addObject(std::move(clonedNodeData));
            }
        }
    }
}

template<class OBJ> void AABBTree<OBJ>::preUpdateObjectCallback(AABBNode<OBJ>&) {
    //can be override
}

/**
 * @param objectsAABBoxHit [out] Objects AABBox hit by the aabbox
 */
template<class OBJ> void AABBTree<OBJ>::aabboxQuery(const AABBox<float>& aabbox, std::vector<OBJ>& objectsAABBoxHit) const {
    browseNodes.clear();
    if (rootNode) {
        browseNodes.push_back(rootNode.get());
    }

    for (std::size_t i = 0; i < browseNodes.size(); ++i) { //tree traversal: pre-order (iterative)
        const AABBNode<OBJ>* currentNode = browseNodes[i];

        if (currentNode->getAABBox().collideWithAABBox(aabbox)) {
            if (currentNode->isLeaf()) {
                objectsAABBoxHit.push_back(currentNode->getNodeData().getNodeObject());
            } else {
                browseNodes.push_back(currentNode->getRightChild());
                browseNodes.push_back(currentNode->getLeftChild());
            }
        }
    }
}

/**
 * @param objectsAABBoxHitRay [out] Objects AABBox hit by the ray
 */
template<class OBJ> void AABBTree<OBJ>::rayQuery(const Ray<float>& ray, std::vector<OBJ>& objectsAABBoxHitRay) const {
    browseNodes.clear();
    if (rootNode) {
        browseNodes.push_back(rootNode.get());
    }

    for (std::size_t i = 0; i < browseNodes.size(); ++i) { //tree traversal: pre-order (iterative)
        const AABBNode<OBJ>* currentNode = browseNodes[i];

        if (currentNode->getAABBox().collideWithRay(ray)) {
            if (currentNode->isLeaf()) {
                objectsAABBoxHitRay.push_back(currentNode->getNodeData().getNodeObject());
            } else {
                browseNodes.push_back(currentNode->getRightChild());
                browseNodes.push_back(currentNode->getLeftChild());
            }
        }
    }
}

/**
 * Enlarge each node box of a specified size and process a classical ray test. This method provide similar result to a OBB test but with better performance.
 * @param enlargeNodeBoxHalfSize Specify the size of the enlargement. A size of 0.5 will enlarge the node box from 1.0 (0.5 on left and 0.5 on right).
 * @param objectToExclude Object to exclude from result
 * @param objectsAABBoxHitEnlargedRay [out] Objects AABBox hit by the enlarged ray
 */
template<class OBJ> void AABBTree<OBJ>::enlargedRayQuery(const Ray<float>& ray, float enlargeNodeBoxHalfSize, const OBJ objectToExclude,
                               std::vector<OBJ>& objectsAABBoxHitEnlargedRay) const {
    browseNodes.clear();
    if (rootNode) {
        browseNodes.push_back(rootNode.get());
    }

    for (std::size_t i = 0; i < browseNodes.size(); ++i) { //tree traversal: pre-order (iterative)
        const AABBNode<OBJ>* currentNode = browseNodes[i];

        AABBox<float> extendedNodeAABBox = currentNode->getAABBox().enlarge(enlargeNodeBoxHalfSize, enlargeNodeBoxHalfSize);
        if (extendedNodeAABBox.collideWithRay(ray)) {
            if (currentNode->isLeaf()) {
                OBJ object = currentNode->getNodeData().getNodeObject();
                if (object != objectToExclude) {
                    objectsAABBoxHitEnlargedRay.push_back(object);
                }
            } else {
                browseNodes.push_back(currentNode->getRightChild());
                browseNodes.push_back(currentNode->getLeftChild());
            }
        }
    }
}
