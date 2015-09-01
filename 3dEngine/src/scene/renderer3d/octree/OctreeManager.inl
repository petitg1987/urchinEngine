template<class TOctreeable> OctreeManager<TOctreeable>::OctreeManager(int depth) :
		isInitialized(false),
		overflowSize(ConfigService::instance()->getFloatValue("octree.overflowSize")),
		depth(depth),
		mainOctree(nullptr)
{
	if(overflowSize < -std::numeric_limits<float>::epsilon())
	{
		throw std::domain_error("Parameter overflow size cannot be negative.");
	}
	
	overflowSize += 0.001f; //add offset to avoid rounding problem when overflow size is 0.0f.
}

template<class TOctreeable> OctreeManager<TOctreeable>::~OctreeManager()
{
	std::set<TOctreeable *> allOctreeables;
	if(mainOctree!=nullptr)
	{
		mainOctree->getAllOctreeables(allOctreeables);
		delete mainOctree;
	}
}

template<class TOctreeable> void OctreeManager<TOctreeable>::initialize()
{
	if(isInitialized)
	{
		throw std::runtime_error("Octree manager is already initialized.");
	}

	std::set<TOctreeable *> emptyOctreeable;
	buildOctree(emptyOctreeable);
	
	isInitialized = true;
}

template<class TOctreeable> void OctreeManager<TOctreeable>::setDepth(int depth)
{
	this->depth = depth;

	//gets all octreeables from the current octree
	std::set<TOctreeable *> allOctreeables;
	if(mainOctree!=nullptr)
	{
		mainOctree->getAllOctreeables(allOctreeables);
	}

	//rebuild the octree
	buildOctree(allOctreeables);
}

template<class TOctreeable> void OctreeManager<TOctreeable>::buildOctree(std::set<TOctreeable *> &octreeables)
{	
	//gets the position and the size of the scene (begin)
	Point3<float> minScene(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Point3<float> maxScene(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	for(typename std::set<TOctreeable *>::iterator itOctreeable=octreeables.begin(); itOctreeable!=octreeables.end(); ++itOctreeable)
	{
		const Point3<float> &bboxMin = (*itOctreeable)->getAABBox().getMin();
		if(minScene.X > bboxMin.X)
		{
			minScene.X = bboxMin.X;
		}
		if(minScene.Y > bboxMin.Y)
		{
			minScene.Y = bboxMin.Y;
		}
		if(minScene.Z > bboxMin.Z)
		{
			minScene.Z = bboxMin.Z;
		}

		const Point3<float> &bboxMax = (*itOctreeable)->getAABBox().getMax();
		if(maxScene.X < bboxMax.X)
		{
			maxScene.X = bboxMax.X;
		}
		if(maxScene.Y < bboxMax.Y)
		{
			maxScene.Y = bboxMax.Y;
		}
		if(maxScene.Z < bboxMax.Z)
		{
			maxScene.Z = bboxMax.Z;
		}
	}
	
	Vector3<float> size = (minScene.vector(maxScene));
	size.X += overflowSize * 2.0f;
	size.Y += overflowSize * 2.0f;
	size.Z += overflowSize * 2.0f;
	
	Point3<float> position = minScene;
	position.X -= overflowSize;
	position.Y -= overflowSize;
	position.Z -= overflowSize;
	//gets the position and the size of the scene (end)
	
	delete mainOctree;
	mainOctree = new Octree<TOctreeable>(position, size, depth);
	
	//adds the octreeables to the octree
	for(typename std::set<TOctreeable *>::iterator itOctreeable=octreeables.begin(); itOctreeable!=octreeables.end(); ++itOctreeable)
	{
		addOctreeable(*itOctreeable);
	}

	//notify observers
	notifyObservers(this, OCTREE_BUILT);
}

template<class TOctreeable> void OctreeManager<TOctreeable>::addOctreeable(TOctreeable *octreeable)
{		
	bool resized = resizeOctree(octreeable);
	if(!resized)
	{
		mainOctree->addOctreeable(octreeable, true);
	}
}

template<class TOctreeable> void OctreeManager<TOctreeable>::removeOctreeable(TOctreeable *octreeable)
{
	//keep size in variable because we remove references during looping
	int refOctreeSize = octreeable->getRefOctree().size();
	
	//delete from end to start: more faster for a vector
	for(int i=refOctreeSize-1; i>=0; --i)
	{
		octreeable->getRefOctree()[i]->removeOctreeable(octreeable, true);
	}
}

template<class TOctreeable> std::set<TOctreeable *> OctreeManager<TOctreeable>::getOctreeables() const
{
	std::set<TOctreeable *> allOctreeables;
	mainOctree->getAllOctreeables(allOctreeables);
	
	return allOctreeables;
}

template<class TOctreeable> const Octree<TOctreeable> &OctreeManager<TOctreeable>::getMainOctree() const
{
	return *mainOctree;
}

template<class TOctreeable> std::set<TOctreeable *> OctreeManager<TOctreeable>::getOctreeablesIn(const ConvexObject3D<float> &convexObject)
{
	return getOctreeablesIn(convexObject, AcceptAllFilter<TOctreeable>());
}

template<class TOctreeable> std::set<TOctreeable *> OctreeManager<TOctreeable>::getOctreeablesIn(const ConvexObject3D<float> &convexObject, const OctreeableFilter<TOctreeable> &filter)
{
	refreshOctreeables();
	
	std::set<TOctreeable *> visibleOctreeables;
	mainOctree->getOctreeablesIn(visibleOctreeables, convexObject, filter);
	return visibleOctreeables;
}

template<class TOctreeable> void OctreeManager<TOctreeable>::refreshOctreeables()
{
	//check if the octreeables are moving
	if(mainOctree!=nullptr)
	{
		std::set<TOctreeable *> allOctreeables;
		mainOctree->getAllOctreeables(allOctreeables);

		//1. remove octreeables which have been moved
		for(typename std::set<TOctreeable *>::iterator it=allOctreeables.begin();it!=allOctreeables.end();++it)
		{
			if((*it)->hasNeedUpdate())
			{
				removeOctreeable((*it));
			}
		}

		//2. add the octreeables which have been moved and recreate the octree if necessary
		for(typename std::set<TOctreeable *>::iterator it=allOctreeables.begin();it!=allOctreeables.end();++it)
		{
			if((*it)->hasNeedUpdate())
			{
				addOctreeable((*it));
				(*it)->setToUpdate(false);
			}
		}
	}
}

template<class TOctreeable> bool OctreeManager<TOctreeable>::resizeOctree(TOctreeable *newOctreeable)
{
	if(mainOctree!=nullptr)
	{
		//need to resize ?
		const Point3<float> &minOctree = mainOctree->getAABBox().getMin();
		const Point3<float> &maxOctree = mainOctree->getAABBox().getMax();

		if(	newOctreeable->getAABBox().getMin().X >= minOctree.X && newOctreeable->getAABBox().getMin().Y >= minOctree.Y &&
			newOctreeable->getAABBox().getMin().Z >= minOctree.Z && newOctreeable->getAABBox().getMax().X <= maxOctree.X &&
			newOctreeable->getAABBox().getMax().Y <= maxOctree.Y && newOctreeable->getAABBox().getMax().Z <= maxOctree.Z
		)
		{ //there is no need to resize
			return false;
		}
	}

	//gets all octreeables from the current octree
	std::set<TOctreeable *> allOctreeables;
	if(mainOctree!=nullptr)
	{
		mainOctree->getAllOctreeables(allOctreeables);
	}
	allOctreeables.insert(newOctreeable);

	//rebuild the octree
	buildOctree(allOctreeables);
	
	return true;
}

#ifdef _DEBUG
	template<class TOctreeable> void OctreeManager<TOctreeable>::drawOctree(const Matrix4<float> &projectionMatrix, const Matrix4<float> &viewMatrix) const
	{
		mainOctree->drawNode(projectionMatrix, viewMatrix);
	}
#endif
