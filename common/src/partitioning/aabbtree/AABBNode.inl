template<class OBJ> AABBNode<OBJ>::AABBNode(AABBNodeData<OBJ>* nodeData) :
        nodeData(nodeData),
        parentNode(nullptr) {
    this->children[0] = nullptr;
    this->children[1] = nullptr;
}

template<class OBJ> AABBNode<OBJ>::~AABBNode() {
    delete nodeData;
}

template<class OBJ> AABBNodeData<OBJ> *AABBNode<OBJ>::getNodeData() const {
    return nodeData;
}

template<class OBJ> bool AABBNode<OBJ>::isLeaf() const {
    return children[0] == nullptr;
}

template<class OBJ> bool AABBNode<OBJ>::isRoot() const {
    return parentNode == nullptr;
}

template<class OBJ> void AABBNode<OBJ>::setParent(std::shared_ptr<AABBNode<OBJ>> parentNode) {
    this->parentNode = parentNode;
}

template<class OBJ> void AABBNode<OBJ>::setLeftChild(std::shared_ptr<AABBNode<OBJ>> leftChild) {
    this->children[0] = leftChild;
    if (leftChild) {
        this->children[0]->setParent(this->shared_from_this());
    }
}

template<class OBJ> AABBNode<OBJ>* AABBNode<OBJ>::getLeftChild() const {
    return children[0].get();
}

template<class OBJ> void AABBNode<OBJ>::setRightChild(std::shared_ptr<AABBNode<OBJ>> rightChild) {
    this->children[1] = rightChild;
    if (rightChild) {
        this->children[1]->setParent(this->shared_from_this());
    }
}

template<class OBJ> AABBNode<OBJ>* AABBNode<OBJ>::getRightChild() const {
    return children[1].get();
}

/**
 * Returns fat AABBox for leaf and bounding box for branch
 */
template<class OBJ> const AABBox<float> &AABBNode<OBJ>::getAABBox() const {
    return aabbox;
}

template<class OBJ> void AABBNode<OBJ>::updateAABBox(float fatMargin) {
    if (isLeaf()) {
        Point3<float> fatMargin3(fatMargin, fatMargin, fatMargin);
        AABBox<float> objectBox = nodeData->retrieveObjectAABBox();

        aabbox = AABBox<float>(objectBox.getMin()-fatMargin3, objectBox.getMax()+fatMargin3);
    } else {
        aabbox = children[0]->getAABBox().merge(children[1]->getAABBox());
    }
}

template<class OBJ> void AABBNode<OBJ>::clearNodeData() {
    nodeData = nullptr;
}

template<class OBJ> std::shared_ptr<AABBNode<OBJ>> AABBNode<OBJ>::getParent() const {
    return parentNode;
}

template<class OBJ> std::shared_ptr<AABBNode<OBJ>> AABBNode<OBJ>::getLeftChildSmartPtr() const {
    return children[0];
}

template<class OBJ> std::shared_ptr<AABBNode<OBJ>> AABBNode<OBJ>::getRightChildSmartPtr() const {
    return children[1];
}

template<class OBJ> std::shared_ptr<AABBNode<OBJ>> AABBNode<OBJ>::getSibling() const {
    return parentNode->getLeftChild() == this ? parentNode->getRightChildSmartPtr() : parentNode->getLeftChildSmartPtr();
}
