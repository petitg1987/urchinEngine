#include <stdexcept>
#include <algorithm>

#include <collision/narrowphase/algorithm/CollisionAlgorithmSelector.h>
#include <collision/narrowphase/algorithm/SphereSphereCollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/SphereBoxCollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/ConvexConvexCollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/CompoundAnyCollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/ConcaveAnyCollisionAlgorithm.h>

namespace urchin {

    CollisionAlgorithmSelector::CollisionAlgorithmSelector() :
            algorithmPool(nullptr),
            collisionAlgorithmBuilderMatrix() {
        initializeCollisionAlgorithmBuilderMatrix();
        for (unsigned int i = 0; i < CollisionShape3D::SHAPE_MAX; ++i) {
            for (unsigned int j = 0; j < CollisionShape3D::SHAPE_MAX; ++j) {
                if (!collisionAlgorithmBuilderMatrix[i][j]) {
                    deleteCollisionAlgorithmBuilderMatrix();
                    throw std::invalid_argument("Collision algorithm builder not initialized for shape type: " + std::to_string(i) + " and " + std::to_string(j) + ".");
                }
            }
        }

        initializeAlgorithmPool();
    }

    CollisionAlgorithmSelector::~CollisionAlgorithmSelector() {
        delete algorithmPool;

        deleteCollisionAlgorithmBuilderMatrix();
    }

    void CollisionAlgorithmSelector::initializeCollisionAlgorithmBuilderMatrix() {
        //initialize matrix with nullptr collision algorithm builder
        for (auto & builderMatrixLine : collisionAlgorithmBuilderMatrix) {
            for (auto & builderMatrixColumn : builderMatrixLine) {
                builderMatrixColumn = nullptr;
            }
        }

        //specific algorithms
        collisionAlgorithmBuilderMatrix[CollisionShape3D::SPHERE_SHAPE][CollisionShape3D::SPHERE_SHAPE] = new SphereSphereCollisionAlgorithm::Builder();
        collisionAlgorithmBuilderMatrix[CollisionShape3D::SPHERE_SHAPE][CollisionShape3D::BOX_SHAPE] = new SphereBoxCollisionAlgorithm::Builder();
        collisionAlgorithmBuilderMatrix[CollisionShape3D::BOX_SHAPE][CollisionShape3D::SPHERE_SHAPE] = new SphereBoxCollisionAlgorithm::Builder();
        initializeConcaveAlgorithm();
        initializeCompoundAlgorithm();

        //generic algorithm (convex hull vs. convex hull)
        for (auto& builderMatrixLine : collisionAlgorithmBuilderMatrix) {
            for (auto& builderMatrixColumn : builderMatrixLine) {
                if (!builderMatrixColumn) {
                    builderMatrixColumn = new ConvexConvexCollisionAlgorithm::Builder();
                }
            }
        }
    }

    void CollisionAlgorithmSelector::initializeConcaveAlgorithm() {
        //heightfield shape
        for (unsigned int shapeId = 0; shapeId < CollisionShape3D::SHAPE_MAX; ++shapeId) {
            collisionAlgorithmBuilderMatrix[CollisionShape3D::HEIGHTFIELD_SHAPE][shapeId] = new ConcaveAnyCollisionAlgorithm::Builder();

            if (shapeId != CollisionShape3D::HEIGHTFIELD_SHAPE) {
                collisionAlgorithmBuilderMatrix[shapeId][CollisionShape3D::HEIGHTFIELD_SHAPE] = new ConcaveAnyCollisionAlgorithm::Builder();
            }
        }
    }

    void CollisionAlgorithmSelector::initializeCompoundAlgorithm() {
        for (unsigned int shapeId = 0; shapeId < CollisionShape3D::SHAPE_MAX; ++shapeId) {
            if (!collisionAlgorithmBuilderMatrix[CollisionShape3D::COMPOUND_SHAPE][shapeId]) {
                collisionAlgorithmBuilderMatrix[CollisionShape3D::COMPOUND_SHAPE][shapeId] = new CompoundAnyCollisionAlgorithm::Builder();
            }

            if (shapeId != CollisionShape3D::COMPOUND_SHAPE && !collisionAlgorithmBuilderMatrix[shapeId][CollisionShape3D::COMPOUND_SHAPE]) {
                collisionAlgorithmBuilderMatrix[shapeId][CollisionShape3D::COMPOUND_SHAPE] = new CompoundAnyCollisionAlgorithm::Builder();
            }
        }
    }

    void CollisionAlgorithmSelector::deleteCollisionAlgorithmBuilderMatrix() {
        for (auto & builderMatrixLine : collisionAlgorithmBuilderMatrix) {
            for (auto & builderMatrixColumn : builderMatrixLine) {
                delete builderMatrixColumn;
            }
        }
    }

    void CollisionAlgorithmSelector::initializeAlgorithmPool() {
        unsigned int maxElementSize = 0;
        for (auto& builderMatrixLine : collisionAlgorithmBuilderMatrix) {
            for (auto& builderMatrixColumn : builderMatrixLine) {
                maxElementSize = std::max(maxElementSize, builderMatrixColumn->getAlgorithmSize());
            }
        }
        unsigned int algorithmPoolSize = ConfigService::instance()->getUnsignedIntValue("narrowPhase.algorithmPoolSize");

        //pool is synchronized because elements are created in narrow phase (= synchronized phase called by different threads) and deleted by different threads outside the narrow phase
        algorithmPool = new SyncFixedSizePool<CollisionAlgorithm>("algorithmPool", maxElementSize, algorithmPoolSize);
    }

    /**
     * @param shape1 Shape or partial shape composing the body 1
     * @param shape2 Shape or partial shape composing the body 2
     */
    std::shared_ptr<CollisionAlgorithm> CollisionAlgorithmSelector::createCollisionAlgorithm(AbstractBody* body1, const CollisionShape3D& shape1,
                                                                                             AbstractBody* body2, const CollisionShape3D& shape2) const {
        CollisionAlgorithmBuilder* collisionAlgorithmBuilder = collisionAlgorithmBuilderMatrix[shape1.getShapeType()][shape2.getShapeType()];
        const std::vector<CollisionShape3D::ShapeType>& firstExpectedShapeType = collisionAlgorithmBuilder->getFirstExpectedShapeType();

        CollisionAlgorithm *collisionAlgorithmPtr;
        if (std::find(firstExpectedShapeType.begin(), firstExpectedShapeType.end(), shape1.getShapeType()) != firstExpectedShapeType.end()) {
            collisionAlgorithmPtr = collisionAlgorithmBuilder->createCollisionAlgorithm(false, ManifoldResult(body1, body2), algorithmPool);
        } else if (std::find(firstExpectedShapeType.begin(), firstExpectedShapeType.end(), shape2.getShapeType()) != firstExpectedShapeType.end()) { //objects must be swap to match algorithm shape types
            collisionAlgorithmPtr = collisionAlgorithmBuilder->createCollisionAlgorithm(true, ManifoldResult(body2, body1), algorithmPool);
        } else {
            throw std::runtime_error("Impossible to initialize collision algorithm for shape types: " + std::to_string(shape1.getShapeType())
                                     + " and " + std::to_string(shape2.getShapeType()));
        }

        std::shared_ptr<CollisionAlgorithm> collisionAlgorithm = std::shared_ptr<CollisionAlgorithm>(collisionAlgorithmPtr, AlgorithmDeleter(algorithmPool));
        collisionAlgorithm->setupCollisionAlgorithmSelector(this);
        return collisionAlgorithm;
    }

    CollisionAlgorithmSelector::AlgorithmDeleter::AlgorithmDeleter(FixedSizePool<CollisionAlgorithm> *const algorithmPool) :
        algorithmPool(algorithmPool) {

    }

    void CollisionAlgorithmSelector::AlgorithmDeleter::operator()(CollisionAlgorithm *const collisionAlgorithm) {
        algorithmPool->deallocate(collisionAlgorithm);
    }

}
