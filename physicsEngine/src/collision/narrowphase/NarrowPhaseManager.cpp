#include "collision/narrowphase/NarrowPhaseManager.h"
#include "shape/CollisionShape3D.h"
#include "shape/CollisionSphereShape.h"
#include "shape/CollisionCompoundShape.h"
#include "shape/CollisionConcaveShape.h"
#include "body/model/RigidBody.h"
#include "object/TemporalObject.h"

namespace urchin {

    NarrowPhaseManager::NarrowPhaseManager(const BodyManager* bodyManager, const BroadPhaseManager* broadPhaseManager) :
            bodyManager(bodyManager),
            broadPhaseManager(broadPhaseManager),
            collisionAlgorithmSelector(new CollisionAlgorithmSelector()),
            bodiesMutex(LockById::getInstance("narrowPhaseBodyIds")) {

    }

    NarrowPhaseManager::~NarrowPhaseManager() {
        delete collisionAlgorithmSelector;
    }

    /**
     * @param dt Delta of time (sec.) between two simulation steps
     * @param overlappingPairs Pairs of bodies potentially colliding
     * @param manifoldResults [OUT] Collision constraints
     */
    void NarrowPhaseManager::process(float dt, const std::vector<OverlappingPair*>& overlappingPairs, std::vector<ManifoldResult>& manifoldResults) {
        ScopeProfiler sp(Profiler::physics(), "narrowPhase");

        processOverlappingPairs(overlappingPairs, manifoldResults);
        processPredictiveContacts(dt, manifoldResults);
    }

    /**
     * Process ghost body. This method can be called in different thread that physics thread.
     * @param ghostBody Ghost body to process
     * @param manifoldResults [OUT] Collision constraints
     */
    void NarrowPhaseManager::processGhostBody(GhostBody* ghostBody, std::vector<ManifoldResult>& manifoldResults) {
        std::vector<OverlappingPair> overlappingPairs = ghostBody->getPairContainer()->retrieveCopyOverlappingPairs();

        for (auto& overlappingPair : overlappingPairs) {
            processOverlappingPair(&overlappingPair, manifoldResults);
        }
    }

    void NarrowPhaseManager::processOverlappingPairs(const std::vector<OverlappingPair*>& overlappingPairs, std::vector<ManifoldResult>& manifoldResults) {
        ScopeProfiler sp(Profiler::physics(), "procOverlapPair");

        for (const auto& overlappingPair : overlappingPairs) {
            processOverlappingPair(overlappingPair, manifoldResults);
        }
    }

    void NarrowPhaseManager::processOverlappingPair(OverlappingPair* overlappingPair, std::vector<ManifoldResult>& manifoldResults) {
        AbstractBody* body1 = overlappingPair->getBody1();
        AbstractBody* body2 = overlappingPair->getBody2();

        if (body1->isActive() || body2->isActive()) {
            ScopeLockById lockBody1(bodiesMutex, body1->getObjectId());
            ScopeLockById lockBody2(bodiesMutex, body2->getObjectId());

            std::shared_ptr<CollisionAlgorithm> collisionAlgorithm = retrieveCollisionAlgorithm(overlappingPair);

            CollisionObjectWrapper collisionObject1(*body1->getShape(), body1->getTransform());
            CollisionObjectWrapper collisionObject2(*body2->getShape(), body2->getTransform());
            collisionAlgorithm->processCollisionAlgorithm(collisionObject1, collisionObject2, true);

            if (collisionAlgorithm->getConstManifoldResult().getNumContactPoints() != 0) {
                manifoldResults.push_back(collisionAlgorithm->getConstManifoldResult());
            }
        }
    }

    std::shared_ptr<CollisionAlgorithm> NarrowPhaseManager::retrieveCollisionAlgorithm(OverlappingPair* overlappingPair) {
        std::shared_ptr<CollisionAlgorithm> collisionAlgorithm = overlappingPair->getCollisionAlgorithm();
        if (!collisionAlgorithm) {
            AbstractBody* body1 = overlappingPair->getBody1();
            AbstractBody* body2 = overlappingPair->getBody2();

            collisionAlgorithm = collisionAlgorithmSelector->createCollisionAlgorithm(body1, body1->getShape().get(), body2, body2->getShape().get());

            overlappingPair->setCollisionAlgorithm(collisionAlgorithm);
        }

        return collisionAlgorithm;
    }

    void NarrowPhaseManager::processPredictiveContacts(float dt, std::vector<ManifoldResult>& manifoldResults) {
        ScopeProfiler sp(Profiler::physics(), "proPrediContact");

        for (auto workBody : bodyManager->getBodies()) {
            RigidBody* body = RigidBody::upCast(workBody);
            if (body && body->isActive()) {
                ScopeLockById lockBody(bodiesMutex, body->getObjectId());

                PhysicsTransform currentTransform = body->getTransform();
                PhysicsTransform newTransform = currentTransform.integrate(body->getLinearVelocity(), body->getAngularVelocity(), dt);

                float ccdMotionThreshold = body->getCcdMotionThreshold();
                float motion = currentTransform.getPosition().vector(newTransform.getPosition()).length();

                if (motion > ccdMotionThreshold) {
                    handleContinuousCollision(body, currentTransform, newTransform, manifoldResults);
                }
            }
        }
    }

    void NarrowPhaseManager::handleContinuousCollision(AbstractBody* body, const PhysicsTransform& from, const PhysicsTransform& to, std::vector<ManifoldResult>& manifoldResults) {
        std::vector<AbstractBody*> bodiesAABBoxHitBody = broadPhaseManager->bodyTest(body, from, to);
        if (!bodiesAABBoxHitBody.empty()) {
            ccd_set ccdResults;

            const auto& bodyShape = body->getShape();
            if (bodyShape->isCompound()) {
                const auto& compoundShape = std::dynamic_pointer_cast<const CollisionCompoundShape>(bodyShape);
                const std::vector<std::shared_ptr<const LocalizedCollisionShape>> & localizedShapes = compoundShape->getLocalizedShapes();
                for (const auto& localizedShape : localizedShapes) {
                    TemporalObject temporalObject(localizedShape->shape.get(), from * localizedShape->transform, to * localizedShape->transform);
                    ccdResults.merge(continuousCollisionTest(temporalObject, bodiesAABBoxHitBody));
                }
            } else if (bodyShape->isConvex()) {
                TemporalObject temporalObject(body->getShape().get(), from, to);
                ccdResults = continuousCollisionTest(temporalObject, bodiesAABBoxHitBody);
            } else {
                throw std::invalid_argument("Unknown shape type category: " + std::to_string(bodyShape->getShapeType()));
            }

            if (!ccdResults.empty()) {
                const std::unique_ptr<ContinuousCollisionResult<float>, AlgorithmResultDeleter>& firstCCDResult = *ccdResults.begin();

                Vector3<float> distanceVector = from.getPosition().vector(to.getPosition()) * firstCCDResult->getTimeToHit();
                float depth = distanceVector.dotProduct(-firstCCDResult->getNormalFromObject2());
                const Point3<float>& hitPointOnObject2 = firstCCDResult->getHitPointOnObject2();
                const Vector3<float>& normalFromObject2 = firstCCDResult->getNormalFromObject2();

                ManifoldResult manifoldResult(body, firstCCDResult->getBody2());
                manifoldResult.addContactPoint(normalFromObject2, hitPointOnObject2, depth, true);

                manifoldResults.push_back(manifoldResult);
            }
        }
    }

    ccd_set NarrowPhaseManager::continuousCollisionTest(const TemporalObject& temporalObject1, const std::vector<AbstractBody*>& bodiesAABBoxHit) const {
        ccd_set continuousCollisionResults;

        for (auto bodyAABBoxHit : bodiesAABBoxHit) {
            if (bodyAABBoxHit->isGhostBody()) {
                //No CCD support for ghost bodies
                continue;
            }
            ScopeLockById lockBody(bodiesMutex, bodyAABBoxHit->getObjectId());

            const auto& bodyShape = bodyAABBoxHit->getShape();
            if (bodyShape->isCompound()) {
                const auto& compoundShape = std::dynamic_pointer_cast<const CollisionCompoundShape>(bodyShape);
                const std::vector<std::shared_ptr<const LocalizedCollisionShape>>& localizedShapes = compoundShape->getLocalizedShapes();
                for (const auto& localizedShape : localizedShapes) {
                    PhysicsTransform fromToObject2 = bodyAABBoxHit->getTransform() * localizedShape->transform;
                    TemporalObject temporalObject2(localizedShape->shape.get(), fromToObject2, fromToObject2);

                    continuousCollisionTest(temporalObject1, temporalObject2, bodyAABBoxHit, continuousCollisionResults);
                }
            } else if (bodyShape->isConvex()) {
                PhysicsTransform fromToObject2 = bodyAABBoxHit->getTransform();
                TemporalObject temporalObject2(bodyShape.get(), fromToObject2, fromToObject2);

                continuousCollisionTest(temporalObject1, temporalObject2, bodyAABBoxHit, continuousCollisionResults);
            } else if (bodyShape->isConcave()) {
                const auto& concaveShape = std::dynamic_pointer_cast<const CollisionConcaveShape>(bodyShape);

                PhysicsTransform inverseTransformObject2 = bodyAABBoxHit->getTransform().inverse();
                AABBox<float> fromAABBoxLocalToObject1 = temporalObject1.getShape()->toAABBox(inverseTransformObject2 * temporalObject1.getFrom());
                AABBox<float> toAABBoxLocalToObject1 = temporalObject1.getShape()->toAABBox(inverseTransformObject2 * temporalObject1.getTo());

                if (temporalObject1.isRay()) {
                    LineSegment3D<float> ray(fromAABBoxLocalToObject1.getMin(), toAABBoxLocalToObject1.getMin());
                    const std::vector<CollisionTriangleShape>& triangles = concaveShape->findTrianglesHitByRay(ray);

                    trianglesContinuousCollisionTest(triangles, temporalObject1, bodyAABBoxHit, continuousCollisionResults);
                } else {
                    AABBox<float> temporalAABBoxLocalToObject1 = fromAABBoxLocalToObject1.merge(toAABBoxLocalToObject1);
                    const std::vector<CollisionTriangleShape>& triangles = concaveShape->findTrianglesInAABBox(temporalAABBoxLocalToObject1);

                    trianglesContinuousCollisionTest(triangles, temporalObject1, bodyAABBoxHit, continuousCollisionResults);
                }
            } else {
                throw std::invalid_argument("Unknown shape type category: " + std::to_string(bodyShape->getShapeType()));
            }
        }

        return continuousCollisionResults;
    }

    /**
     * @param continuousCollisionResults [OUT] In case of collision detected: continuous collision result will be updated with collision details
     */
    void NarrowPhaseManager::trianglesContinuousCollisionTest(const std::vector<CollisionTriangleShape>& triangles, const TemporalObject& temporalObject1,
                                                              AbstractBody* body2, ccd_set& continuousCollisionResults) const {
        for (const auto& triangle : triangles) {
            PhysicsTransform fromToObject2 = body2->getTransform();
            TemporalObject temporalObject2(&triangle, fromToObject2, fromToObject2);

            continuousCollisionTest(temporalObject1, temporalObject2, body2, continuousCollisionResults);
        }
    }

    /**
     * @param continuousCollisionResults [OUT] In case of collision detected: continuous collision result will be updated with collision details
     */
    void NarrowPhaseManager::continuousCollisionTest(const TemporalObject& temporalObject1, const TemporalObject& temporalObject2,
                                                     AbstractBody* body2, ccd_set& continuousCollisionResults) const {
        std::unique_ptr<ContinuousCollisionResult<float>, AlgorithmResultDeleter> continuousCollisionResult = gjkContinuousCollisionAlgorithm
                .calculateTimeOfImpact(temporalObject1, temporalObject2, body2);

        if (continuousCollisionResult) {
            continuousCollisionResults.insert(std::move(continuousCollisionResult));
        }
    }

    ccd_set NarrowPhaseManager::rayTest(const Ray<float>& ray, const std::vector<AbstractBody*>& bodiesAABBoxHitRay) const {
        CollisionSphereShape pointShape(0.0f);
        PhysicsTransform from = PhysicsTransform(ray.getOrigin());
        PhysicsTransform to = PhysicsTransform(ray.computeTo());
        TemporalObject rayCastObject(&pointShape, from, to);

        return continuousCollisionTest(rayCastObject, bodiesAABBoxHitRay);
    }

}
