template<class TOctreeable> Octree<TOctreeable>::Octree(const Point3<float> &position, const Vector3<float> &size, float minSize) :
	bbox(AABBox<float>(position, size))
{
    Vector3<float> sizeChild = size;
    std::vector<float> splitX = {position.X};
    std::vector<float> splitY = {position.Y};
    std::vector<float> splitZ = {position.Z};
    if(size.X/2.0f > minSize)
    {
        sizeChild.X /= 2.0f;
        splitX.push_back(position.X + sizeChild.X);
    }
    if(size.Y/2.0f > minSize)
    {
        sizeChild.Y /= 2.0f;
        splitY.push_back(position.Y + sizeChild.Y);
    }
    if(size.Z/2.0f > minSize)
    {
        sizeChild.Z /= 2.0f;
        splitZ.push_back(position.Z + sizeChild.Z);
    }

    if(splitX.size()==1 && splitY.size()==1 && splitZ.size()==1)
    {
        isLeaf = true;
    }else
    {
        isLeaf = false;

        for (float xValue : splitX)
        {
            for (float yValue : splitY)
            {
                for (float zValue : splitZ)
                {
                    Point3<float> positionChild(xValue, yValue, zValue);
                    children.push_back(new Octree(positionChild, sizeChild, minSize));
                }
            }
        }
    }
}

template<class TOctreeable> Octree<TOctreeable>::~Octree()
{
	if(isLeaf)
	{
		//remove references to this octree
		for(auto &octreeable : octreeables)
		{
            octreeable->removeRefOctree(this);
		}
	}else
	{
		//delete children
		for(auto &child : children)
		{
			delete child;
		}
	}
}

template<class TOctreeable> const AABBox<float> &Octree<TOctreeable>::getAABBox() const
{
	return bbox;
}

template<class TOctreeable> void Octree<TOctreeable>::addOctreeable(TOctreeable *octreeable, bool addRef)
{
	const Point3<float> &minOctree = bbox.getMin();
	const Point3<float> &maxOctree = bbox.getMax();

	if(	octreeable->getAABBox().getMin().X <= maxOctree.X && octreeable->getAABBox().getMax().X >= minOctree.X &&
		octreeable->getAABBox().getMin().Y <= maxOctree.Y && octreeable->getAABBox().getMax().Y >= minOctree.Y &&
		octreeable->getAABBox().getMin().Z <= maxOctree.Z && octreeable->getAABBox().getMax().Z >= minOctree.Z
	)
	{
		if(isLeaf)
		{
			octreeables.push_back(octreeable);
			if(addRef)
			{
				octreeable->addRefOctree(this);
			}
		}else
		{
			for(auto &child : children)
			{
				child->addOctreeable(octreeable, addRef);
			}
		}
	}
}

template<class TOctreeable> void Octree<TOctreeable>::removeOctreeable(TOctreeable *octreeable, bool removeRef)
{
	if(isLeaf)
	{
		auto it = std::find(octreeables.begin(), octreeables.end(), octreeable);
		if(it!=octreeables.end())
		{
			VectorEraser::erase(octreeables, it);
			if(removeRef)
			{
				octreeable->removeRefOctree(this);
			}
		}
	}
}

template<class TOctreeable> void Octree<TOctreeable>::getOctreeablesIn(std::set<TOctreeable *> &visibleOctreeables, const ConvexObject3D<float> &convexObject, const OctreeableFilter<TOctreeable> &filter) const
{
	if(convexObject.collideWithAABBox(bbox))
	{		
		if(isLeaf)
		{
			for(unsigned int i=0;i<octreeables.size();i++)
			{
				if(octreeables[i]->isVisible() && filter.isAccepted(octreeables[i])
						&& convexObject.collideWithAABBox(octreeables[i]->getAABBox()))
				{
					visibleOctreeables.insert(octreeables[i]);
				}
			}
		}else
		{
			for(const auto &child : children)
			{
				child->getOctreeablesIn(visibleOctreeables, convexObject, filter);
			}
		}
	}
}

template<class TOctreeable> void Octree<TOctreeable>::getAllOctreeables(std::set<TOctreeable *> &allOctreeables) const
{
	if(isLeaf)
	{
		allOctreeables.insert(octreeables.begin(), octreeables.end());
	}else
	{
		for(const auto &child : children)
		{
			child->getAllOctreeables(allOctreeables);
		}
	}
}

template<class TOctreeable> void Octree<TOctreeable>::getAllLeafOctrees(std::vector<const Octree<TOctreeable> *> &allOctrees) const
{
	if(isLeaf)
	{
		allOctrees.push_back(this);
	}else
	{
		for(const auto &child : children)
		{
			child->getAllLeafOctrees(allOctrees);
		}
	}
}
