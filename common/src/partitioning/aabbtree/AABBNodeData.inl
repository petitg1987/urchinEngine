template<class OBJ> AABBNodeData<OBJ>::AABBNodeData(OBJ nodeObject) :
        nodeObject(nodeObject) {

}

template<class OBJ> const OBJ& AABBNodeData<OBJ>::getNodeObject() const {
    return nodeObject;
}
