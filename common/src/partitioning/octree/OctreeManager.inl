template<class TOctreeable> OctreeManager<TOctreeable>::OctreeManager(float minSize) :
		overflowSize(ConfigService::instance()->getFloatValue("octree.overflowSize")),
        minSize(minSize),
		mainOctree(nullptr),
        refreshModCount(0),
        postRefreshModCount(0)
{
	if(overflowSize < -std::numeric_limits<float>::epsilon())
	{
		throw std::domain_error("Parameter overflow size cannot be negative.");
	}
	
	overflowSize += 0.001f; //add offset to avoid rounding problem when overflow size is 0.0f.
	
	std::vector<TOctreeable *> emptyOctreeable;
	buildOctree(emptyOctreeable);
}

template<class TOctreeable> OctreeManager<TOctreeable>::~OctreeManager()
{
	if(mainOctree)
	{
        for(const auto &octreeable : getAllOctreeables())
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
	    if(notificationType==TOctreeable::MOVE)
        {
            TOctreeable *octreeable = dynamic_cast<TOctreeable *>(observable);
            movingOctreeables.emplace_back(octreeable);
        }
	}
}

template<class TOctreeable> void OctreeManager<TOctreeable>::buildOctree(std::vector<TOctreeable *> &octreeables)
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
        browseNodes.clear();
        browseNodes.push_back(mainOctree);
        for(std::size_t i = 0; i < browseNodes.size(); ++i)
        {
            Octree<TOctreeable> *octree = browseNodes[i];

            if (octreeable->getAABBox().collideWithAABBox(octree->getAABBox()))
            {
                if (octree->isLeaf())
                {
                    octree->addOctreeable(octreeable, true);
                } else
                {
                    browseNodes.insert(browseNodes.end(), octree->getChildren().begin(), octree->getChildren().end());
                }
            }
        }
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
	std::vector<TOctreeable *> allOctreeables;
	if(mainOctree)
	{
		mainOctree->getAllOctreeables(allOctreeables);
	}

	//rebuild the octree
	buildOctree(allOctreeables);
}

template<class TOctreeable> void OctreeManager<TOctreeable>::refreshOctreeables()
{
	ScopeProfiler profiler("3d", "refreshOctreeab");

	if(mainOctree)
	{
        movingOctreeables.erase(std::unique(movingOctreeables.begin(), movingOctreeables.end() ), movingOctreeables.end());

        for(auto &movingOctreeable : movingOctreeables)
		{
			removeOctreeable(movingOctreeable);
		}

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
    #endif
    refreshModCount++;
}

template<class TOctreeable> void OctreeManager<TOctreeable>::postRefreshOctreeables()
{
    for(auto &movingOctreeable : movingOctreeables)
	{
        movingOctreeable->onMoveDone();
	}
	
	movingOctreeables.clear();

    postRefreshModCount++;
}

template<class TOctreeable> const Octree<TOctreeable> &OctreeManager<TOctreeable>::getMainOctree() const
{
	return *mainOctree;
}

template<class TOctreeable> std::vector<const Octree<TOctreeable> *> OctreeManager<TOctreeable>::getAllLeafOctrees() const
{
    std::vector<const Octree<TOctreeable> *> leafOctrees;

    browseNodes.clear();
    browseNodes.push_back(mainOctree);
    for(std::size_t i=0; i<browseNodes.size(); ++i)
    {
        const Octree<TOctreeable> *octree = browseNodes[i];

        if(octree->isLeaf())
        {
            leafOctrees.push_back(octree);
        }else
        {
            browseNodes.insert(browseNodes.end(), octree->getChildren().begin(), octree->getChildren().end());
        }
    }

    return leafOctrees;
}

template<class TOctreeable> std::vector<TOctreeable *> OctreeManager<TOctreeable>::getAllOctreeables() const
{
	std::vector<TOctreeable *> allOctreeables;

	if(mainOctree)
    {
        browseNodes.clear();
        browseNodes.push_back(mainOctree);
        for (std::size_t i = 0; i < browseNodes.size(); ++i)
        {
            const Octree<TOctreeable> *octree = browseNodes[i];

            if (octree->isLeaf())
            {
                for(std::size_t octreeableI=0; octreeableI<octree->getOctreeables().size(); octreeableI++)
                {
                    TOctreeable *octreeable = octree->getOctreeables()[octreeableI];

                    if(!octreeable->isProcessed())
                    {
                        octreeable->setProcessed(true);
                        allOctreeables.emplace_back(octreeable);
                    }
                }
            } else
            {
                browseNodes.insert(browseNodes.end(), octree->getChildren().begin(), octree->getChildren().end());
            }
        }
    }

    std::for_each(allOctreeables.begin(), allOctreeables.end(), [](TOctreeable *o){o->setProcessed(false);});

	return allOctreeables;
}

template<class TOctreeable> void OctreeManager<TOctreeable>::getOctreeablesIn(const ConvexObject3D<float> &convexObject,
        std::vector<TOctreeable *> &octreeables) const
{
	getOctreeablesIn(convexObject, octreeables, AcceptAllFilter<TOctreeable>());
}

template<class TOctreeable> void OctreeManager<TOctreeable>::getOctreeablesIn(const ConvexObject3D<float> &convexObject,
        std::vector<TOctreeable *> &visibleOctreeables, const OctreeableFilter<TOctreeable> &filter) const
{
    ScopeProfiler profiler("3d", "getOctreeables");

	browseNodes.clear();
	browseNodes.push_back(mainOctree);
	for(std::size_t i=0; i<browseNodes.size(); ++i)
	{
		const Octree<TOctreeable> *octree = browseNodes[i];

		if(convexObject.collideWithAABBox(octree->getAABBox()))
		{
			if(octree->isLeaf())
			{
				for(std::size_t octreeableI=0; octreeableI<octree->getOctreeables().size(); octreeableI++)
				{
					TOctreeable *octreeable = octree->getOctreeables()[octreeableI];

					if(octreeable->isVisible() && !octreeable->isProcessed() && filter.isAccepted(octreeable, convexObject))
					{
						octreeable->setProcessed(true);
						visibleOctreeables.push_back(octreeable);
					}
				}
			}else
			{
                browseNodes.insert(browseNodes.end(), octree->getChildren().begin(), octree->getChildren().end());
			}
		}
	}

	std::for_each(visibleOctreeables.begin(), visibleOctreeables.end(), [](TOctreeable *o){o->setProcessed(false);});
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
	std::vector<TOctreeable *> allOctreeables = getAllOctreeables();
	allOctreeables.emplace_back(newOctreeable);

	//rebuild the octree
	buildOctree(allOctreeables);

	return true;
}
