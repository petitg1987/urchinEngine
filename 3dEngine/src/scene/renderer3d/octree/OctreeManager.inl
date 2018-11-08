template<class TOctreeable> OctreeManager<TOctreeable>::OctreeManager(float minSize) :
		overflowSize(ConfigService::instance()->getFloatValue("octree.overflowSize")),
        minSize(minSize),
		mainOctree(nullptr)
{
	if(overflowSize < -std::numeric_limits<float>::epsilon())
	{
		throw std::domain_error("Parameter overflow size cannot be negative.");
	}
	
	overflowSize += 0.001f; //add offset to avoid rounding problem when overflow size is 0.0f.
	
	std::unordered_set<TOctreeable *> emptyOctreeable;
	buildOctree(emptyOctreeable);
	
	#ifdef _DEBUG
		refreshModCount = postRefreshModCount = 0;
	#endif
}

template<class TOctreeable> OctreeManager<TOctreeable>::~OctreeManager()
{
	if(mainOctree)
	{
		std::unordered_set<TOctreeable *> allOctreeables;
		mainOctree->getAllOctreeables(allOctreeables);

        for(const auto &octreeable : allOctreeables)
		{
			removeOctreeable(octreeable);
		}
		
		delete mainOctree;
	}
}

template<class TOctreeable> void OctreeManager<TOctreeable>::notify(Observable *observable, int notificationType)
{
	if(dynamic_cast<TOctreeable *>(observable))
	{
		switch(notificationType)
		{
			case TOctreeable::MOVE:
			{
				TOctreeable *octreeable = dynamic_cast<TOctreeable *>(observable);
				movingOctreeables.insert(octreeable);
			}
		}
	}
}

template<class TOctreeable> void OctreeManager<TOctreeable>::buildOctree(std::unordered_set<TOctreeable *> &octreeables)
{
	if(!octreeables.empty())
	{
		Point3<float> minScene(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		Point3<float> maxScene(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

		for (const auto &octreeable : octreeables)
		{
			const Point3<float> &bboxMin = octreeable->getAABBox().getMin();
			if (minScene.X > bboxMin.X)
			{
				minScene.X = bboxMin.X;
			}
			if (minScene.Y > bboxMin.Y)
			{
				minScene.Y = bboxMin.Y;
			}
			if (minScene.Z > bboxMin.Z)
			{
				minScene.Z = bboxMin.Z;
			}

			const Point3<float> &bboxMax = octreeable->getAABBox().getMax();
			if (maxScene.X < bboxMax.X)
			{
				maxScene.X = bboxMax.X;
			}
			if (maxScene.Y < bboxMax.Y)
			{
				maxScene.Y = bboxMax.Y;
			}
			if (maxScene.Z < bboxMax.Z)
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

		delete mainOctree;
		mainOctree = new Octree<TOctreeable>(position, size, minSize);

		for(auto &octreeable : octreeables)
		{
			addOctreeable(octreeable);
		}
	}else
	{
		delete mainOctree;
		mainOctree = new Octree<TOctreeable>(Point3<float>(0.0, 0.0, 0.0), Vector3<float>(1.0, 1.0, 1.0), minSize);
	}

	notifyObservers(this, OCTREE_BUILT);
}

template<class TOctreeable> void OctreeManager<TOctreeable>::addOctreeable(TOctreeable *octreeable)
{		
	bool resized = resizeOctree(octreeable);
	if(!resized)
	{
		mainOctree->addOctreeable(octreeable, true);
	}
	
	octreeable->addObserver(this, TOctreeable::MOVE);
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
	
	octreeable->removeObserver(this, TOctreeable::MOVE);
}

template<class TOctreeable> void OctreeManager<TOctreeable>::updateMinSize(float minSize)
{
	this->minSize = minSize;

	//gets all octreeables from the current octree
	std::unordered_set<TOctreeable *> allOctreeables;
	if(mainOctree)
	{
		mainOctree->getAllOctreeables(allOctreeables);
	}

	//rebuild the octree
	buildOctree(allOctreeables);
}

template<class TOctreeable> void OctreeManager<TOctreeable>::refreshOctreeables()
{
	ScopeProfiler profiler("3d", "refreshOctreeables");

	if(mainOctree)
	{
		//1. remove octreeables which have been moved
        for(auto &movingOctreeable : movingOctreeables)
		{
			removeOctreeable(movingOctreeable);
		}

		//2. add the octreeables which have been moved and recreate the octree if necessary
        for(auto &movingOctreeable : movingOctreeables)
		{
			addOctreeable(movingOctreeable);
		}
	}
	
	#ifdef _DEBUG
		if(refreshModCount!=postRefreshModCount)
		{
			throw std::runtime_error("Methods 'refreshOctreeables' and 'postRefreshOctreeables' must be called the same number of times: " +
					std::to_string(refreshModCount) + " <-> " + std::to_string(postRefreshModCount));
		}
		refreshModCount++;
	#endif
}

template<class TOctreeable> void OctreeManager<TOctreeable>::postRefreshOctreeables()
{
    for(auto &movingOctreeable : movingOctreeables)
	{
        movingOctreeable->onMoveDone();
	}
	
	movingOctreeables.clear();
	
	#ifdef _DEBUG
		postRefreshModCount++;
	#endif
}

template<class TOctreeable> const Octree<TOctreeable> &OctreeManager<TOctreeable>::getMainOctree() const
{
	return *mainOctree;
}

/**
 * Returns octreeables which move. This list is valid from the time the octreeables move until a call to postRefreshOctreeables method is done.
 */
template<class TOctreeable> const std::unordered_set<TOctreeable *> &OctreeManager<TOctreeable>::getMovingOctreeables() const
{
	return movingOctreeables;
}

template<class TOctreeable> std::unordered_set<TOctreeable *> OctreeManager<TOctreeable>::getAllOctreeables() const
{
	std::unordered_set<TOctreeable *> allOctreeables;
	mainOctree->getAllOctreeables(allOctreeables);
	
	return allOctreeables;
}

template<class TOctreeable> void OctreeManager<TOctreeable>::getOctreeablesIn(const ConvexObject3D<float> &convexObject,
        std::unordered_set<TOctreeable *> &octreeables) const
{
	getOctreeablesIn(convexObject, octreeables, AcceptAllFilter<TOctreeable>());
}

template<class TOctreeable> void OctreeManager<TOctreeable>::getOctreeablesIn(const ConvexObject3D<float> &convexObject,
        std::unordered_set<TOctreeable *> &octreeables, const OctreeableFilter<TOctreeable> &filter) const
{
    ScopeProfiler profiler("3d", "getOctreeablesIn");

	mainOctree->getOctreeablesIn(octreeables, convexObject, filter);
}

template<class TOctreeable> bool OctreeManager<TOctreeable>::resizeOctree(TOctreeable *newOctreeable)
{
	if(mainOctree)
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
	std::unordered_set<TOctreeable *> allOctreeables;
	if(mainOctree)
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
		std::vector<const Octree<TOctreeable> *> leafOctrees;
        mainOctree->getAllLeafOctrees(leafOctrees);

		std::vector<AABBox<float>> aabboxes;
		aabboxes.reserve(leafOctrees.size());

		for(const auto &leafOctree : leafOctrees)
		{
			aabboxes.push_back(leafOctree->getAABBox());
		}

		AABBoxModel aabboxModel(aabboxes);
		aabboxModel.onCameraProjectionUpdate(projectionMatrix);
		aabboxModel.display(viewMatrix);
	}
#endif
