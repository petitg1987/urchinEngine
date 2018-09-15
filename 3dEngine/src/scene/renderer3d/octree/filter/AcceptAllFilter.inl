template<class TOctreeable> inline bool AcceptAllFilter<TOctreeable>::isAccepted(const TOctreeable *const octreeable, const ConvexObject3D<float> &convexObject) const
{
    //Info: to filter by individual octreeables (instead of all octreeables belong to an octree): convexObject.collideWithAABBox(octreeable->getAABBox())
	return true;
}
