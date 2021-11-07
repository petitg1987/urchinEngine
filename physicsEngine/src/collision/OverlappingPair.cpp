#include <utility>

#include <collision/OverlappingPair.h>

namespace urchin {

    OverlappingPair::OverlappingPair(std::shared_ptr<AbstractBody> body1, std::shared_ptr<AbstractBody> body2) :
            body1(std::move(body1)),
            body2(std::move(body2)),
            bodiesId(computeBodiesId(*this->body1, *this->body2)),
            collisionAlgorithm(nullptr) {

    }

    /**
    * @param bodiesId Unique id representing both bodies
    */
    OverlappingPair::OverlappingPair(std::shared_ptr<AbstractBody> body1, std::shared_ptr<AbstractBody> body2, uint_fast64_t bodiesId) :
            body1(std::move(body1)),
            body2(std::move(body2)),
            bodiesId(bodiesId),
            collisionAlgorithm(nullptr) {

    }

    OverlappingPair::OverlappingPair(const OverlappingPair& src) :
            body1(src.body1),
            body2(src.body2),
            bodiesId(src.bodiesId),
            collisionAlgorithm(nullptr) {

    }

    /**
      * @param index Index of body to return. Index must be '0' for body1 or '1' for body2.
      */
    AbstractBody& OverlappingPair::getBody(unsigned int index) const {
        assert(index == 0 || index == 1);
        return index == 0 ? *body1 : *body2;
    }

    AbstractBody& OverlappingPair::getBody1() const {
        return *body1;
    }

    const std::shared_ptr<AbstractBody>& OverlappingPair::getBody1Ptr() const {
        return body1;
    }

    AbstractBody& OverlappingPair::getBody2() const {
        return *body2;
    }

    const std::shared_ptr<AbstractBody>& OverlappingPair::getBody2Ptr() const {
        return body2;
    }

    /**
     * Computes an unique key for both bodies
     */
    uint_fast64_t OverlappingPair::computeBodiesId(const AbstractBody& body1, const AbstractBody& body2) {
        uint_fast64_t key;
        if (body1.getObjectId() > body2.getObjectId()) {
            key = body2.getObjectId();
            key = key << 32u;
            key += body1.getObjectId();
        } else {
            key = body1.getObjectId();
            key = key << 32u;
            key += body2.getObjectId();
        }

        return key;
    }

    /**
     * @return Unique id representing both bodies
     */
    uint_fast64_t OverlappingPair::getBodiesId() const {
        return bodiesId;
    }

    void OverlappingPair::setCollisionAlgorithm(std::unique_ptr<CollisionAlgorithm, AlgorithmDeleter> collisionAlgorithm) {
        this->collisionAlgorithm = std::move(collisionAlgorithm);
    }

    CollisionAlgorithm* OverlappingPair::getCollisionAlgorithm() const {
        return collisionAlgorithm.get();
    }

}
