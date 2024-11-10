template<class T> Octree<T>::Octree(const Point3<float>& position, const Vector3<float>& size, float minSize) :
        bbox(AABBox(position, size)) {
    Vector3<float> sizeChild = size;
    std::vector splitX = {position.X};
    std::vector splitY = {position.Y};
    std::vector splitZ = {position.Z};
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
                    Point3 positionChild(xValue, yValue, zValue);
                    children.push_back(std::make_unique<Octree>(positionChild, sizeChild, minSize));
                }
            }
        }
    }
}

template<class T> Octree<T>::~Octree() {
    if (bIsLeaf) {
        //remove references to this octree
        for (auto& octreeable : octreeables) {
            octreeable->removeRefOctree(this);
        }
    }
}

template<class T> const AABBox<float> &Octree<T>::getAABBox() const {
    return bbox;
}

template<class T> bool Octree<T>::isLeaf() const {
    return bIsLeaf;
}

template<class T> const std::vector<std::unique_ptr<Octree<T>>>& Octree<T>::getChildren() const {
    return children;
}

template<class T> const std::vector<std::shared_ptr<T>>& Octree<T>::getOctreeables() const {
    return octreeables;
}

template<class T> void Octree<T>::addOctreeable(std::shared_ptr<T> octreeable, bool addRef) {
    assert(bIsLeaf);

    T* octreeablePtr = octreeable.get();
    octreeables.push_back(std::move(octreeable));
    if (addRef) {
        octreeablePtr->addRefOctree(this);
    }
}

template<class T> std::shared_ptr<T> Octree<T>::removeOctreeable(T* octreeable, bool removeRef) {
    assert(bIsLeaf);

    auto it = std::ranges::find_if(octreeables, [&octreeable](const auto& o){return o.get() == octreeable;});
    if (it != octreeables.end()) {
        std::shared_ptr<T> removedOctreeable = *it; //keep a copy before remove the octreeable
        VectorUtil::erase(octreeables, it);
        if (removeRef) {
            octreeable->removeRefOctree(this);
        }
        return removedOctreeable;
    }
    return std::shared_ptr<T>(nullptr);
}
