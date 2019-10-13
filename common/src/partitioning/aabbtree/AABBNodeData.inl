template<class OBJ> AABBNodeData<OBJ>::AABBNodeData(OBJ nodeObject) :
        nodeObject(nodeObject)
{

}

template<class OBJ> OBJ AABBNodeData<OBJ>::getNodeObject() const
{
    return nodeObject;
}
