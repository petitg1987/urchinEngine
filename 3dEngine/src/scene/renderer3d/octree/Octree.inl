template<class TOctreeable> Octree<TOctreeable>::Octree(const Point3<float> &position, const Vector3<float> &size, int depth) :
	bbox(AABBox<float>(position, size)),
	depth(depth)
{
	//creates 8 children
	if(depth > 0)
	{
		const Vector3<float> &sizeChild = size / 2.0f;
		
		//child 1
		Point3<float> posChild = position;
		children.push_back(new Octree(posChild, sizeChild, depth-1));
		
		//child 2
		posChild = position;
		posChild.X = posChild.X + sizeChild.X;
		children.push_back(new Octree(posChild, sizeChild, depth-1));
		
		//child 3
		posChild = position;
		posChild.Z = posChild.Z + sizeChild.Z;
		children.push_back(new Octree(posChild, sizeChild, depth-1));
		
		//child 4
		posChild = position;
		posChild.X = posChild.X + sizeChild.X;
		posChild.Z = posChild.Z + sizeChild.Z;
		children.push_back(new Octree(posChild, sizeChild, depth-1));
		
		//child 5
		posChild = position;
		posChild.Y = posChild.Y + sizeChild.Y;
		children.push_back(new Octree(posChild, sizeChild, depth-1));
		
		//child 6
		posChild = position;
		posChild.X = posChild.X + sizeChild.X;
		posChild.Y = posChild.Y + sizeChild.Y;
		children.push_back(new Octree(posChild, sizeChild, depth-1));
		
		//child 7
		posChild = position;
		posChild.Y = posChild.Y + sizeChild.Y;
		posChild.Z = posChild.Z + sizeChild.Z;
		children.push_back(new Octree(posChild, sizeChild, depth-1));
		
		//child 8
		posChild = position;
		posChild.X = posChild.X + sizeChild.X;
		posChild.Y = posChild.Y + sizeChild.Y;
		posChild.Z = posChild.Z + sizeChild.Z;
		children.push_back(new Octree(posChild, sizeChild, depth-1));
	}
}

template<class TOctreeable> Octree<TOctreeable>::~Octree()
{
	if(depth==0)
	{
		//remove references to this octree
		for(typename std::vector<TOctreeable *>::iterator it = octreeables.begin(); it!=octreeables.end(); ++it)
		{
			(*it)->removeRefOctree(this);
		}
	}else
	{
		//delete children
		for(typename std::vector<Octree *>::iterator it=children.begin(); it!=children.end(); ++it)
		{
			delete (*it);
		}
	}
}

template<class TOctreeable> const AABBox<float> &Octree<TOctreeable>::getAABBox() const
{
	return bbox;
}

template<class TOctreeable> int Octree<TOctreeable>::getDepth() const
{
	return depth;
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
		if(depth == 0)
		{ //leaf of the octree
			octreeables.push_back(octreeable);
			if(addRef)
			{
				octreeable->addRefOctree(this);
			}
		}else
		{
			for(typename std::vector<Octree *>::iterator it=children.begin(); it!=children.end(); ++it)
			{
				(*it)->addOctreeable(octreeable, addRef);
			}
		}
	}
}

template<class TOctreeable> void Octree<TOctreeable>::removeOctreeable(TOctreeable *octreeable, bool removeRef)
{
	if(depth==0)
	{
		typename std::vector<TOctreeable *>::iterator it = std::find(octreeables.begin(), octreeables.end(), octreeable);
		if(it!=octreeables.end())
		{
			octreeables.erase(it);
			if(removeRef)
			{
				octreeable->removeRefOctree(this);
			}
		}
	}
}

template<class TOctreeable> void Octree<TOctreeable>::getOctreeablesIn(std::set<TOctreeable *> &visibleOctreeables, const ConvexObject3D<float> &convexObject, const OctreeableFilter<TOctreeable> &filter)
{
	if(convexObject.collideWithAABBox(bbox))
	{		
		if(depth==0)
		{ //leaf of the octree
			for(unsigned int i=0;i<octreeables.size();i++)
			{
				if(octreeables[i]->isVisible() && filter.isAccepted(octreeables[i]))
				{
					visibleOctreeables.insert(octreeables[i]);
				}
			}
		}else
		{
			for(typename std::vector<Octree *>::iterator it=children.begin(); it!=children.end(); ++it)
			{
				(*it)->getOctreeablesIn(visibleOctreeables, convexObject, filter);
			}
		}
	}
}

template<class TOctreeable> void Octree<TOctreeable>::getAllOctreeables(std::set<TOctreeable *> &allOctreeables)
{
	if(depth==0)
	{ //leaf of the octree
		allOctreeables.insert(octreeables.begin(), octreeables.end());
	}else
	{
		for(typename std::vector<Octree *>::iterator it=children.begin(); it!=children.end(); ++it)
		{
			(*it)->getAllOctreeables(allOctreeables);
		}
	}
}

#ifdef _DEBUG
	template<class TOctreeable> void Octree<TOctreeable>::drawNode(const Matrix4<float> &projectionMatrix, const Matrix4<float> &viewMatrix) const
	{
		if(depth==0)
		{
			AABBoxModel aabboxModel(bbox);
			aabboxModel.onCameraProjectionUpdate(projectionMatrix);
			aabboxModel.display(viewMatrix);
		}else
		{
			for(typename std::vector<Octree *>::const_iterator it=children.begin(); it!=children.end(); ++it)
			{
				(*it)->drawNode(projectionMatrix, viewMatrix);
			}
		}
	}
#endif