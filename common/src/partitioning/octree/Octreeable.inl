template<class TOctreeable> Octreeable<TOctreeable>::Octreeable() :
    bIsMovingInOctree(false),
    bIsVisible(true),
    bIsProcessed(false) {

}

template<class TOctreeable> Octreeable<TOctreeable>::Octreeable(const Octreeable<TOctreeable>& octreeable) :
    bIsMovingInOctree(false),
    bIsVisible(octreeable.isVisible()),
    bIsProcessed(octreeable.isProcessed()) {

}

template<class TOctreeable> Octreeable<TOctreeable>::~Octreeable() {
    //remove references to this octreeable
    for (auto it = refOctree.begin(); it!=refOctree.end(); ++it) {
        TOctreeable* toctreeable = static_cast<TOctreeable*>(this);
        (*it)->removeOctreeable(toctreeable, false);
    }
}

/**
 * Method to be called when octreeable moving
 */
template<class TOctreeable> void Octreeable<TOctreeable>::notifyOctreeableMove() {
    notifyObservers(this, Octreeable::MOVE);

    if (refOctree.size() > 0) { //octreeable can move in an octree only if it's attached to an octree
        bIsMovingInOctree = true;
    }
}

/**
 * Method called at post process when octreeable is moving
 */
template<class TOctreeable> void Octreeable<TOctreeable>::onMoveDone() {
    bIsMovingInOctree = false;
}

/**
 * Return true when octreeable moves in an octree. True is even returned if octreeable doesn't change of leaf octree.
 * If octreeable is not attached to an octree: false is always returned.
 */
template<class TOctreeable> bool Octreeable<TOctreeable>::isMovingInOctree() const {
    return bIsMovingInOctree;
}

template<class TOctreeable> void Octreeable<TOctreeable>::setVisible(bool isVisible) {
    bIsVisible = isVisible;
}

template<class TOctreeable> bool Octreeable<TOctreeable>::isVisible() const {
    return bIsVisible;
}

template<class TOctreeable> void Octreeable<TOctreeable>::setProcessed(bool isProcessed) {
    bIsProcessed = isProcessed;
}

template<class TOctreeable> bool Octreeable<TOctreeable>::isProcessed() const {
    return bIsProcessed;
}

template<class TOctreeable> const std::vector<Octree<TOctreeable>*> &Octreeable<TOctreeable>::getRefOctree() const {
    return refOctree;
}

template<class TOctreeable> void Octreeable<TOctreeable>::addRefOctree(Octree<TOctreeable>* octree) {
    refOctree.push_back(octree);
}

template<class TOctreeable> void Octreeable<TOctreeable>::removeRefOctree(Octree<TOctreeable>* octree) {
    auto it = std::find(refOctree.begin(), refOctree.end(), octree);
    if (it!=refOctree.end()) {
        VectorEraser::erase(refOctree, it);
    } else {
        throw std::invalid_argument("Impossible to find the reference octree to remove.");
    }
}
