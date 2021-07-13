template<class T> bool AcceptAllFilter<T>::isAccepted(const T* const, const ConvexObject3D<float>&) const {
    //Info: to filter by individual octreeables (instead of all octreeables belong to an octree): convexObject.collideWithAABBox(octreeable->getAABBox())
    return true;
}
