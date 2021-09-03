template<class T> Octreeable<T>::Octreeable() :
        bIsMovingInOctree(false),
        bIsVisible(true),
        bIsProcessed(false) {

}

template<class T> Octreeable<T>::Octreeable(const Octreeable<T>& octreeable) :
        bIsMovingInOctree(false),
        bIsVisible(octreeable.isVisible()),
        bIsProcessed(octreeable.isProcessed()) {

}

template<class T> Octreeable<T>::~Octreeable() {
    //remove references to this octreeable
    for (auto it = refOctree.begin(); it != refOctree.end(); ++it) {
        T* toctreeable = static_cast<T*>(this);
        (*it)->removeOctreeable(toctreeable, false);
    }
}

/**
 * Method to be called when octreeable moving
 */
template<class T> void Octreeable<T>::notifyOctreeableMove() {
    notifyObservers(this, Octreeable::MOVE);

    if (refOctree.size() > 0) { //octreeable can move in an octree only if it's attached to an octree
        bIsMovingInOctree = true;
    }
}

/**
 * Method called at post process when octreeable is moving
 */
template<class T> void Octreeable<T>::onMoveDone() {
    bIsMovingInOctree = false;
}

/**
 * Return true when octreeable moves in an octree. True is even returned if octreeable doesn't change of leaf octree.
 * If octreeable is not attached to an octree: false is always returned.
 */
template<class T> bool Octreeable<T>::isMovingInOctree() const {
    return bIsMovingInOctree;
}

template<class T> void Octreeable<T>::setVisible(bool isVisible) {
    bIsVisible = isVisible;
}

template<class T> bool Octreeable<T>::isVisible() const {
    return bIsVisible;
}

template<class T> void Octreeable<T>::setProcessed(bool isProcessed) {
    bIsProcessed = isProcessed;
}

template<class T> bool Octreeable<T>::isProcessed() const {
    return bIsProcessed;
}

template<class T> const std::vector<Octree<T>*> &Octreeable<T>::getRefOctree() const {
    return refOctree;
}

template<class T> void Octreeable<T>::addRefOctree(Octree<T>* octree) {
    refOctree.push_back(octree);
}

template<class T> void Octreeable<T>::removeRefOctree(Octree<T>* octree) {
    auto it = std::find(refOctree.begin(), refOctree.end(), octree);
    if (it != refOctree.end()) {
        VectorUtil::erase(refOctree, it);
    } else {
        throw std::invalid_argument("Impossible to find the reference octree to remove.");
    }
}
