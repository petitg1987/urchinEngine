template<class T> OctreeManager<T>::OctreeManager(float minSize) :
        overflowSize(ConfigService::instance()->getFloatValue("octree.overflowSize")),
        minSize(minSize),
        mainOctree(nullptr),
        refreshModCount(0),
        postRefreshModCount(0) {
    if (overflowSize < -std::numeric_limits<float>::epsilon()) {
        throw std::domain_error("Parameter overflow size cannot be negative.");
    }

    overflowSize += 0.001f; //add offset to avoid rounding problem when overflow size is 0.0f.

    std::vector<std::shared_ptr<T>> emptyOctreeable;
    buildOctree(emptyOctreeable);
}

template<class T> OctreeManager<T>::~OctreeManager() {
    if (mainOctree) {
        for (const auto& octreeable : getAllOctreeables()) {
            removeOctreeable(octreeable.get());
        }

        delete mainOctree;
    }
}

template<class T> void OctreeManager<T>::notify(Observable* observable, int notificationType) {
    if (dynamic_cast<T*>(observable)) {
        if (notificationType == T::MOVE) {
            T* octreeable = dynamic_cast<T*>(observable);
            movingOctreeables.emplace_back(octreeable);
        }
    }
}

template<class T> void OctreeManager<T>::buildOctree(std::vector<std::shared_ptr<T>>& octreeables) {
    if (!octreeables.empty()) {
        Point3<float> minScene(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Point3<float> maxScene(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

        for (const auto& octreeable : octreeables) {
            const Point3<float>& bboxMin = octreeable->getAABBox().getMin();
            if (minScene.X > bboxMin.X) {
                minScene.X = bboxMin.X;
            }
            if (minScene.Y > bboxMin.Y) {
                minScene.Y = bboxMin.Y;
            }
            if (minScene.Z > bboxMin.Z) {
                minScene.Z = bboxMin.Z;
            }

            const Point3<float>& bboxMax = octreeable->getAABBox().getMax();
            if (maxScene.X < bboxMax.X) {
                maxScene.X = bboxMax.X;
            }
            if (maxScene.Y < bboxMax.Y) {
                maxScene.Y = bboxMax.Y;
            }
            if (maxScene.Z < bboxMax.Z) {
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
        mainOctree = new Octree<T>(position, size, minSize);

        for (auto& octreeable : octreeables) {
            addOctreeable(octreeable);
        }
    } else {
        delete mainOctree;
        mainOctree = new Octree<T>(Point3<float>(0.0f, 0.0f, 0.0f), Vector3<float>(1.0f, 1.0f, 1.0f), minSize);
    }

    notifyObservers(this, OCTREE_BUILT);
}

template<class T> void OctreeManager<T>::addOctreeable(std::shared_ptr<T> octreeable) {
    bool resized = resizeOctree(octreeable);
    if (!resized) {
        browseNodes.clear();
        browseNodes.push_back(mainOctree);
        for (std::size_t i = 0; i < browseNodes.size(); ++i) {
            Octree<T>* octree = browseNodes[i];

            if (octreeable->getAABBox().collideWithAABBox(octree->getAABBox())) {
                if (octree->isLeaf()) {
                    octree->addOctreeable(octreeable, true);
                } else {
                    browseNodes.insert(browseNodes.end(), octree->getChildren().begin(), octree->getChildren().end());
                }
            }
        }
    }

    octreeable->addObserver(this, T::MOVE);
}

template<class T> std::shared_ptr<T> OctreeManager<T>::removeOctreeable(T* octreeable) {
    //keep size in variable because we remove references during looping
    int refOctreeSize = (int)octreeable->getRefOctree().size();

    //remove from end to start: more faster for a vector
    std::shared_ptr<T> removedOctreeable(nullptr);
    for (int i = refOctreeSize - 1; i >= 0; --i) {
        removedOctreeable = octreeable->getRefOctree()[(std::size_t)i]->removeOctreeable(octreeable, true);
    }

    octreeable->removeObserver(this, T::MOVE);
    return removedOctreeable;
}

template<class T> void OctreeManager<T>::updateMinSize(float minSize) {
    this->minSize = minSize;

    //gets all octreeables from the current octree
    std::vector<T*> allOctreeables;
    if (mainOctree) {
        mainOctree->getAllOctreeables(allOctreeables);
    }

    //rebuild the octree
    buildOctree(allOctreeables);
}

template<class T> void OctreeManager<T>::refreshOctreeables() {
    ScopeProfiler sp(Profiler::graphic(), "refreshOctreeab");

    if (mainOctree) {
        movingOctreeables.erase(std::unique(movingOctreeables.begin(), movingOctreeables.end() ), movingOctreeables.end()); //TODO is it working ? (not sorted ?)

        std::vector<std::shared_ptr<T>> removedOctreeables;
        removedOctreeables.reserve(movingOctreeables.size());
        for (auto& movingOctreeable : movingOctreeables) {
            removedOctreeables.push_back(removeOctreeable(movingOctreeable));
        }

        for (auto& movingOctreeable : removedOctreeables) {
            addOctreeable(movingOctreeable);
        }
    }

    assert(refreshModCount == postRefreshModCount); //methods 'refreshOctreeables' and 'postRefreshOctreeables' must be called the same number of times

    refreshModCount++;
}

template<class T> void OctreeManager<T>::postRefreshOctreeables() {
    for (auto& movingOctreeable : movingOctreeables) {
        movingOctreeable->onMoveDone();
    }

    movingOctreeables.clear();

    postRefreshModCount++;
}

template<class T> const Octree<T> &OctreeManager<T>::getMainOctree() const {
    return *mainOctree;
}

template<class T> std::vector<const Octree<T>*> OctreeManager<T>::getAllLeafOctrees() const {
    std::vector<const Octree<T>*> leafOctrees;

    browseNodes.clear();
    browseNodes.push_back(mainOctree);
    for (std::size_t i = 0; i < browseNodes.size(); ++i) {
        const Octree<T>* octree = browseNodes[i];

        if (octree->isLeaf()) {
            leafOctrees.push_back(octree);
        } else {
            browseNodes.insert(browseNodes.end(), octree->getChildren().begin(), octree->getChildren().end());
        }
    }

    return leafOctrees;
}

template<class T> std::vector<std::shared_ptr<T>> OctreeManager<T>::getAllOctreeables() const {
    std::vector<std::shared_ptr<T>> allOctreeables;

    if (mainOctree) {
        browseNodes.clear();
        browseNodes.push_back(mainOctree);
        for (std::size_t i = 0; i < browseNodes.size(); ++i) {
            const Octree<T>* octree = browseNodes[i];

            if (octree->isLeaf()) {
                for (std::size_t octreeableI = 0; octreeableI < octree->getOctreeables().size(); octreeableI++) {
                    std::shared_ptr<T> octreeable = octree->getOctreeables()[octreeableI];

                    if (!octreeable->isProcessed()) {
                        octreeable->setProcessed(true);
                        allOctreeables.emplace_back(std::move(octreeable));
                    }
                }
            } else {
                browseNodes.insert(browseNodes.end(), octree->getChildren().begin(), octree->getChildren().end());
            }
        }
    }

    std::for_each(allOctreeables.begin(), allOctreeables.end(), [](const std::shared_ptr<T>& o){o->setProcessed(false);});

    return allOctreeables;
}

template<class T> void OctreeManager<T>::getOctreeablesIn(const ConvexObject3D<float>& convexObject, std::vector<T*>& octreeables) const {
    getOctreeablesIn(convexObject, octreeables, AcceptAllFilter<T>());
}

template<class T> void OctreeManager<T>::getOctreeablesIn(const ConvexObject3D<float>& convexObject,
        std::vector<T*>& visibleOctreeables, const OctreeableFilter<T>& filter) const {
    ScopeProfiler sp(Profiler::graphic(), "getOctreeables");

    browseNodes.clear();
    browseNodes.push_back(mainOctree);
    for (std::size_t i = 0; i < browseNodes.size(); ++i) {
        const Octree<T>* octree = browseNodes[i];

        if (convexObject.collideWithAABBox(octree->getAABBox())) {
            if (octree->isLeaf()) {
                for (std::size_t octreeableI = 0; octreeableI < octree->getOctreeables().size(); octreeableI++) {
                    std::shared_ptr<T> octreeable = octree->getOctreeables()[octreeableI];

                    if (octreeable->isVisible() && !octreeable->isProcessed() && filter.isAccepted(octreeable.get(), convexObject)) {
                        octreeable->setProcessed(true);
                        visibleOctreeables.push_back(octreeable.get());
                    }
                }
            } else {
                browseNodes.insert(browseNodes.end(), octree->getChildren().begin(), octree->getChildren().end());
            }
        }
    }

    std::for_each(visibleOctreeables.begin(), visibleOctreeables.end(), [](T* o){o->setProcessed(false);});
}

template<class T> bool OctreeManager<T>::resizeOctree(std::shared_ptr<T> newOctreeable) {
    if (mainOctree) {
        //need to resize ?
        const Point3<float>& minOctree = mainOctree->getAABBox().getMin();
        const Point3<float>& maxOctree = mainOctree->getAABBox().getMax();

        if (    newOctreeable->getAABBox().getMin().X >= minOctree.X && newOctreeable->getAABBox().getMin().Y >= minOctree.Y &&
            newOctreeable->getAABBox().getMin().Z >= minOctree.Z && newOctreeable->getAABBox().getMax().X <= maxOctree.X &&
            newOctreeable->getAABBox().getMax().Y <= maxOctree.Y && newOctreeable->getAABBox().getMax().Z <= maxOctree.Z
        ) { //there is no need to resize
            return false;
        }
    }

    //gets all octreeables from the current octree
    std::vector<std::shared_ptr<T>> allOctreeables = getAllOctreeables();
    allOctreeables.emplace_back(std::move(newOctreeable));

    //rebuild the octree
    buildOctree(allOctreeables);

    return true;
}
