template<class TOctreeable> Octree<TOctreeable>::Octree(const Point3<float>& position, const Vector3<float>& size, float minSize) :
    bbox(AABBox<float>(position, size)) {
    Vector3<float> sizeChild = size;
    std::vector<float> splitX = {position.X};
    std::vector<float> splitY = {position.Y};
    std::vector<float> splitZ = {position.Z};
    if (size.X / 2.0f > minSize) {
        sizeChild.X /= 2.0f;
        splitX.push_back(position.X + sizeChild.X);
    }
    if (size.Y / 2.0f > minSize) {
        sizeChild.Y /= 2.0f;
        splitY.push_back(position.Y + sizeChild.Y);
    }
    if (size.Z / 2.0f > minSize) {
        sizeChild.Z /= 2.0f;
        splitZ.push_back(position.Z + sizeChild.Z);
    }

    if (splitX.size() == 1 && splitY.size() == 1 && splitZ.size() == 1) {
        bIsLeaf = true;
    } else {
        bIsLeaf = false;

        for (float xValue : splitX) {
            for (float yValue : splitY) {
                for (float zValue : splitZ) {
                    Point3<float> positionChild(xValue, yValue, zValue);
                    children.push_back(new Octree(positionChild, sizeChild, minSize));
                }
            }
        }
    }
}

template<class TOctreeable> Octree<TOctreeable>::~Octree() {
    if (bIsLeaf) {
        //remove references to this octree
        for (auto& octreeable : octreeables) {
            octreeable->removeRefOctree(this);
        }
    } else {
        //remove children
        for (auto& child : children) {
            delete child;
        }
    }
}

template<class TOctreeable> const AABBox<float> &Octree<TOctreeable>::getAABBox() const {
    return bbox;
}

template<class TOctreeable> bool Octree<TOctreeable>::isLeaf() const {
    return bIsLeaf;
}

template<class TOctreeable> const std::vector<Octree<TOctreeable>*> &Octree<TOctreeable>::getChildren() const {
    return children;
}

template<class TOctreeable> const std::vector<std::shared_ptr<TOctreeable>> &Octree<TOctreeable>::getOctreeables() const {
    return octreeables;
}

template<class TOctreeable> void Octree<TOctreeable>::addOctreeable(std::shared_ptr<TOctreeable> octreeable, bool addRef) {
    assert(bIsLeaf);

    TOctreeable* octreeablePtr = octreeable.get();
    octreeables.push_back(std::move(octreeable));
    if (addRef) {
        octreeablePtr->addRefOctree(this);
    }
}

template<class TOctreeable> std::shared_ptr<TOctreeable> Octree<TOctreeable>::removeOctreeable(TOctreeable* octreeable, bool removeRef) {
    assert(bIsLeaf);

    auto it = std::find_if(octreeables.begin(), octreeables.end(), [&octreeable](const auto& o){return o.get() == octreeable;});
    if (it != octreeables.end()) {
        std::shared_ptr<TOctreeable> removedOctreeable = *it; //keep a copy before remove the octreeable
        VectorUtil::erase(octreeables, it);
        if (removeRef) {
            octreeable->removeRefOctree(this);
        }
        return removedOctreeable;
    }
    return std::shared_ptr<TOctreeable>(nullptr);
}
