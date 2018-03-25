template<class TOctreeable> inline bool AcceptAllFilter<TOctreeable>::isAccepted(const TOctreeable *const octreeable, const ConvexObject3D<float> &convexObject) const
{
    //return convexObject.collideWithAABBox(octreeable->getAABBox());
	return true;
}
