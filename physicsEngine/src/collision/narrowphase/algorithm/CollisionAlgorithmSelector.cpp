#include <stdexcept>
#include <algorithm>

#include <collision/narrowphase/algorithm/CollisionAlgorithmSelector.h>
#include <collision/narrowphase/algorithm/SphereSphereCollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/SphereBoxCollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/ConvexConvexCollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/CompoundAnyCollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/ConcaveAnyCollisionAlgorithm.h>

namespace urchin {

    AlgorithmDeleter::AlgorithmDeleter(std::shared_ptr<FixedSizePool<CollisionAlgorithm>> algorithmPool) :
            algorithmPool(std::move(algorithmPool)) {

    }

    void AlgorithmDeleter::operator()(CollisionAlgorithm *const collisionAlgorithm) {
        algorithmPool->deallocate(collisionAlgorithm);
    }

    CollisionAlgorithmSelector::CollisionAlgorithmSelector() :
            algorithmPool(std::unique_ptr<SyncFixedSizePool<CollisionAlgorithm>>(nullptr)) {
        initializeCollisionAlgorithmBuilderMatrix();
        for (unsigned int i = 0; i < CollisionShape3D::SHAPE_MAX; ++i) {
            for (unsigned int j = 0; j < CollisionShape3D::SHAPE_MAX; ++j) {
                if (!collisionAlgorithmBuilderMatrix[i][j]) {
                    throw std::invalid_argument("Collision algorithm builder not initialized for shape type: " + std::to_string(i) + " and " + std::to_string(j) + ".");
                }
            }
        }

        initializeAlgorithmPool();
    }

    void CollisionAlgorithmSelector::initializeCollisionAlgorithmBuilderMatrix() {
        //initialize matrix with nullptr collision algorithm builder
        for (auto& builderMatrixLine : collisionAlgorithmBuilderMatrix) {
            for (auto& builderMatrixColumn : builderMatrixLine) {
                builderMatrixColumn = nullptr;
            }
        }

        //specific algorithms
        collisionAlgorithmBuilderMatrix[CollisionShape3D::SPHERE_SHAPE][CollisionShape3D::SPHERE_SHAPE] = std::make_unique<SphereSphereCollisionAlgorithm::Builder>();
        collisionAlgorithmBuilderMatrix[CollisionShape3D::SPHERE_SHAPE][CollisionShape3D::BOX_SHAPE] = std::make_unique<SphereBoxCollisionAlgorithm::Builder>();
        collisionAlgorithmBuilderMatrix[CollisionShape3D::BOX_SHAPE][CollisionShape3D::SPHERE_SHAPE] = std::make_unique<SphereBoxCollisionAlgorithm::Builder>();
        initializeConcaveAlgorithm();
        initializeCompoundAlgorithm();

        //generic algorithm (convex hull vs. convex hull)
        for (auto& builderMatrixLine : collisionAlgorithmBuilderMatrix) {
            for (auto& builderMatrixColumn : builderMatrixLine) {
                if (!builderMatrixColumn) {
                    builderMatrixColumn = std::make_unique<ConvexConvexCollisionAlgorithm::Builder>();
                }
            }
        }
    }

    void CollisionAlgorithmSelector::initializeConcaveAlgorithm() {
        //heightfield shape
        for (unsigned int shapeId = 0; shapeId < CollisionShape3D::SHAPE_MAX; ++shapeId) {
            collisionAlgorithmBuilderMatrix[CollisionShape3D::HEIGHTFIELD_SHAPE][shapeId] = std::make_unique<ConcaveAnyCollisionAlgorithm::Builder>();

            if (shapeId != CollisionShape3D::HEIGHTFIELD_SHAPE) {
                collisionAlgorithmBuilderMatrix[shapeId][CollisionShape3D::HEIGHTFIELD_SHAPE] = std::make_unique<ConcaveAnyCollisionAlgorithm::Builder>();
            }
        }
    }

    void CollisionAlgorithmSelector::initializeCompoundAlgorithm() {
        for (unsigned int shapeId = 0; shapeId < CollisionShape3D::SHAPE_MAX; ++shapeId) {
            if (!collisionAlgorithmBuilderMatrix[CollisionShape3D::COMPOUND_SHAPE][shapeId]) {
                collisionAlgorithmBuilderMatrix[CollisionShape3D::COMPOUND_SHAPE][shapeId] = std::make_unique<CompoundAnyCollisionAlgorithm::Builder>();
            }

            if (shapeId != CollisionShape3D::COMPOUND_SHAPE && !collisionAlgorithmBuilderMatrix[shapeId][CollisionShape3D::COMPOUND_SHAPE]) {
                collisionAlgorithmBuilderMatrix[shapeId][CollisionShape3D::COMPOUND_SHAPE] = std::make_unique<CompoundAnyCollisionAlgorithm::Builder>();
            }
        }
    }

    void CollisionAlgorithmSelector::initializeAlgorithmPool() {
        unsigned int maxElementSize = 0;
        for (const auto& builderMatrixLine : collisionAlgorithmBuilderMatrix) {
            for (const auto& builderMatrixColumn : builderMatrixLine) {
                maxElementSize = std::max(maxElementSize, builderMatrixColumn->getAlgorithmSize());
            }
        }
        unsigned int algorithmPoolSize = ConfigService::instance().getUnsignedIntValue("narrowPhase.algorithmPoolSize");

        //pool is synchronized because elements are created in narrow phase (= synchronized phase called by different threads) and deleted by different threads outside the narrow phase
        algorithmPool = std::make_unique<SyncFixedSizePool<CollisionAlgorithm>>("algorithmPool", maxElementSize, algorithmPoolSize);
    }

    /**
     * @param shape1 Shape or partial shape composing the body 1
     * @param shape2 Shape or partial shape composing the body 2
     */
    std::unique_ptr<CollisionAlgorithm, AlgorithmDeleter> CollisionAlgorithmSelector::createCollisionAlgorithm(
            AbstractBody& body1, const CollisionShape3D& shape1, AbstractBody& body2, const CollisionShape3D& shape2) const {
        auto& collisionAlgorithmBuilder = collisionAlgorithmBuilderMatrix[shape1.getShapeType()][shape2.getShapeType()];
        const std::vector<CollisionShape3D::ShapeType>& firstExpectedShapeType = collisionAlgorithmBuilder->getFirstExpectedShapeType();

        CollisionAlgorithm *collisionAlgorithmPtr;
        if (std::ranges::find(firstExpectedShapeType, shape1.getShapeType()) != firstExpectedShapeType.end()) {
            collisionAlgorithmPtr = collisionAlgorithmBuilder->createCollisionAlgorithm(false, ManifoldResult(body1, body2), *algorithmPool);
        } else if (std::ranges::find(firstExpectedShapeType, shape2.getShapeType()) != firstExpectedShapeType.end()) { //objects must be swap to match algorithm shape types
            collisionAlgorithmPtr = collisionAlgorithmBuilder->createCollisionAlgorithm(true, ManifoldResult(body2, body1), *algorithmPool);
        } else {
            throw std::runtime_error("Impossible to initialize collision algorithm for shape types: " + std::to_string(shape1.getShapeType())
                                     + " and " + std::to_string(shape2.getShapeType()));
        }

        auto collisionAlgorithm = std::unique_ptr<CollisionAlgorithm, AlgorithmDeleter>(collisionAlgorithmPtr, AlgorithmDeleter(algorithmPool));
        collisionAlgorithm->setupCollisionAlgorithmSelector(this);
        return collisionAlgorithm;
    }

}
