
template<class OBJ> AABBTree<OBJ>::AABBTree(float fatMargin) :
        fatMargin(fatMargin),
        rootNode(nullptr)
{

}

template<class OBJ> AABBTree<OBJ>::~AABBTree()
{
    delete rootNode;
}

template <class OBJ> AABBNode<OBJ> *AABBTree<OBJ>::getRootNode() const
{
    return rootNode;
}

template <class OBJ> AABBNodeData<OBJ> *AABBTree<OBJ>::getNodeData(OBJ *object) const
{
    return objectsNode.find(object)->second->getNodeData();
}

template <class OBJ> void AABBTree<OBJ>::addObject(AABBNodeData<OBJ> *nodeData)
{
    auto *nodeToInsert = new AABBNode<OBJ>(nodeData);

    if (rootNode)
    {
        nodeToInsert->updateAABBox(fatMargin);
        insertNode(nodeToInsert, rootNode);
    }else
    {
        rootNode = nodeToInsert;
        rootNode->updateAABBox(fatMargin);
    }

    objectsNode[nodeData->getNodeObject()] = nodeToInsert;

    #ifdef _DEBUG
        //printTree(rootNode, 0);
    #endif

    postAddObjectCallback(nodeToInsert);
}

template<class OBJ>  void AABBTree<OBJ>::postAddObjectCallback(AABBNode<OBJ> *)
{
    //can be override
}

template<class OBJ> void AABBTree<OBJ>::insertNode(AABBNode<OBJ> *nodeToInsert, AABBNode<OBJ> *currentNode)
{
    if (currentNode->isLeaf())
    {
        auto *newParent = new AABBNode<OBJ>(nullptr);
        replaceNode(currentNode, newParent);
        newParent->setLeftChild(nodeToInsert);
        newParent->setRightChild(currentNode);
        newParent->updateAABBox(fatMargin);
    }else
    {
        const AABBox<float> &leftAABBox = currentNode->getLeftChild()->getAABBox();
        float volumeDiffLeft = leftAABBox.merge(nodeToInsert->getAABBox()).getVolume() - leftAABBox.getVolume();

        const AABBox<float> &rightAABBox = currentNode->getRightChild()->getAABBox();
        float volumeDiffRight = rightAABBox.merge(nodeToInsert->getAABBox()).getVolume() - rightAABBox.getVolume();

        if(volumeDiffLeft < volumeDiffRight)
        {
            insertNode(nodeToInsert, currentNode->getLeftChild());
        }else
        {
            insertNode(nodeToInsert, currentNode->getRightChild());
        }

        currentNode->updateAABBox(fatMargin);
    }
}

template<class OBJ> void AABBTree<OBJ>::replaceNode(AABBNode<OBJ> *nodeToReplace, AABBNode<OBJ> *newNode)
{
    if(nodeToReplace->getParent())
    {
        if(nodeToReplace->getParent()->getLeftChild()==nodeToReplace)
        {
            nodeToReplace->getParent()->setLeftChild(newNode);
        }else
        {
            nodeToReplace->getParent()->setRightChild(newNode);
        }
    }else
    {
        rootNode = newNode;
        newNode->setParent(nullptr);
    }
}

template<class OBJ> void AABBTree<OBJ>::removeObject(AABBNodeData<OBJ> *nodeData)
{
    AABBNode<OBJ> *nodeToRemove = objectsNode.find(nodeData->getNodeObject())->second;
    preRemoveObjectCallback(nodeToRemove);

    objectsNode.erase(nodeData->getNodeObject());
    removeNode(nodeToRemove);
}

template<class OBJ> void AABBTree<OBJ>::preRemoveObjectCallback(AABBNode<OBJ> *)
{
    //can be override
}

template<class OBJ> void AABBTree<OBJ>::removeNode(AABBNode<OBJ> *nodeToRemove)
{
    AABBNode<OBJ> *parentNode = nodeToRemove->getParent();

    if(!parentNode)
    {
        rootNode = nullptr;
    }else
    {
        AABBNode<OBJ> *sibling = nodeToRemove->getSibling();
        replaceNode(parentNode, sibling);

        parentNode->setLeftChild(nullptr); //avoid child removal
        parentNode->setRightChild(nullptr); //avoid child removal
        delete parentNode;
    }

    delete nodeToRemove;
}

template<class OBJ> void AABBTree<OBJ>::updateObjects()
{
    for(int i=objectsNode.size()-1; i>=0; --i)
    { //loop backward to remove elements
        auto it = objectsNode.begin();
        std::advance(it, i);

        AABBNode<OBJ> *leaf = it->second;
        if(it->second->getNodeData()->isObjectMoving())
        {
            preUpdateObjectCallback(leaf);

            const AABBox<float> &leafFatAABBox = leaf->getAABBox();
            const AABBox<float> &objectAABBox = leaf->getNodeData()->retrieveObjectAABBox();

            if(!leafFatAABBox.include(objectAABBox))
            {
                AABBNodeData<OBJ> *clonedNodeData = leaf->getNodeData()->clone();
                removeObject(leaf->getNodeData());
                addObject(clonedNodeData);
            }
        }
    }
}

template<class OBJ> void AABBTree<OBJ>::preUpdateObjectCallback(AABBNode<OBJ> *)
{
    //can be override
}

/**
 * @param objectsAABBoxHitRay [out] Objects AABBox hit by the ray
 */
template<class OBJ> void AABBTree<OBJ>::rayTest(const Ray<float> &ray, std::vector<OBJ *> &objectsAABBoxHitRay) const
{
    enlargedRayTest(ray, 0.0f, nullptr, objectsAABBoxHitRay);
}

/**
 * Enlarge each node box of a specified size and process a classical ray test. This method provide similar result to a OBB test but with better performance.
 * @param enlargeNodeBoxHalfSize Specify the size of the enlargement. A size of 0.5 will enlarge the node box from 1.0 (0.5 on left and 0.5 on right).
 * @param objectsAABBoxHitEnlargedRay [out] Objects AABBox hit by the enlarged ray
 */
template<class OBJ> void AABBTree<OBJ>::enlargedRayTest(const Ray<float> &ray, float enlargeNodeBoxHalfSize, const OBJ *testedObject,
                               std::vector<OBJ *> &objectsAABBoxHitEnlargedRay) const
{
    browseNodes.clear();
    browseNodes.push_back(rootNode);

    for(std::size_t i=0; i<browseNodes.size(); ++i)
    { //tree traversal: pre-order (iterative)
        const AABBNode<OBJ> *currentNode = browseNodes[i];

        AABBox<float> extendedNodeAABBox = currentNode->getAABBox().enlarge(enlargeNodeBoxHalfSize, enlargeNodeBoxHalfSize);
        if(extendedNodeAABBox.collideWithRay(ray))
        {
            if (currentNode->isLeaf())
            {
                OBJ *object = currentNode->getNodeData()->getNodeObject();
                if(object!=testedObject)
                {
                    objectsAABBoxHitEnlargedRay.push_back(object);
                }
            }else
            {
                browseNodes.push_back(currentNode->getRightChild());
                browseNodes.push_back(currentNode->getLeftChild());
            }
        }
    }
}

#ifdef _DEBUG
    template<class OBJ> void AABBTree<OBJ>::printTree(AABBNode<OBJ> *node, unsigned int indentLevel)
    { //tree traversal: pre-order (recursive)
        if(node->isLeaf())
        {
            std::cout<<std::string(indentLevel, ' ')<<"- Leaf: "<<node->getNodeData()->getObjectId()<<std::endl;
        }else
        {
            if(!node->getParent())
            {
                std::cout<<std::string(indentLevel, ' ')<<"Root:"<<std::endl;
            }else
            {
                std::cout<<std::string(indentLevel, ' ')<<"- Node:"<<std::endl;
            }

            printTree(node->getLeftChild(), indentLevel + 2);
            printTree(node->getRightChild(), indentLevel + 2);
        }

        if(indentLevel==0)
        {
            std::cout<<std::endl;
        }
    }
#endif