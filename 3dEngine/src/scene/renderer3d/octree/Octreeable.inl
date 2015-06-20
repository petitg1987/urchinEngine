template<class TOctreeable> Octreeable<TOctreeable>::Octreeable() :
	bNeedUpdate(false), bIsVisible(true)
{

}

template<class TOctreeable> Octreeable<TOctreeable>::~Octreeable()
{
	//remove references to this octreeable
	for(typename std::vector<Octree<TOctreeable> *>::iterator it = refOctree.begin(); it!=refOctree.end(); ++it)
	{
		TOctreeable *toctreeable = static_cast<TOctreeable *>(this);
		(*it)->removeOctreeable(toctreeable, false);
	}
}

template<class TOctreeable> void Octreeable<TOctreeable>::setToUpdate(bool needUpdate)
{
	bNeedUpdate = needUpdate;
}

template<class TOctreeable> bool Octreeable<TOctreeable>::hasNeedUpdate()
{
	return bNeedUpdate;
}

template<class TOctreeable> void Octreeable<TOctreeable>::setVisible(bool isVisible)
{
	bIsVisible = isVisible;
}

template<class TOctreeable> bool Octreeable<TOctreeable>::isVisible() const
{
	return bIsVisible;
}

template<class TOctreeable> const std::vector<Octree<TOctreeable> *> &Octreeable<TOctreeable>::getRefOctree() const
{
	return refOctree;
}

template<class TOctreeable> void Octreeable<TOctreeable>::addRefOctree(Octree<TOctreeable> *octree)
{
	refOctree.push_back(octree);
}

template<class TOctreeable> void Octreeable<TOctreeable>::removeRefOctree(Octree<TOctreeable> *octree)
{
	typename std::vector<Octree<TOctreeable> *>::iterator it = std::find(refOctree.begin(), refOctree.end(), octree);
	if(it!=refOctree.end())
	{
		refOctree.erase(it);
	}else
	{
		throw std::invalid_argument("Impossible to find the reference octree to remove.");
	}
}

#ifdef _DEBUG
	template<class TOctreeable> void Octreeable<TOctreeable>::drawBBox(const Matrix4<float> &projectionMatrix, const Matrix4<float> &viewMatrix) const
	{
		std::unique_ptr<AABBoxModel> aabboxModel(new AABBoxModel(getAABBox()));
		aabboxModel->onCameraProjectionUpdate(projectionMatrix);
		aabboxModel->display(viewMatrix);
	}
#endif
