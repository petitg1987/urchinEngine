#include <stdexcept>
#include <fstream>

#include "collision/narrowphase/algorithm/CollisionAlgorithmSelector.h"
#include "collision/narrowphase/algorithm/SphereSphereCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/SphereBoxCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/ConvexConvexCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CompoundAnyCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/ConcaveAnyCollisionAlgorithm.h"

namespace urchin
{

	CollisionAlgorithmSelector::CollisionAlgorithmSelector()
	{
		//initialize algorithm builder
		initializeCollisionAlgorithmBuilderMatrix();

		for(unsigned int i=0; i<CollisionShape3D::SHAPE_MAX; ++i)
		{
			for(unsigned int j=0; j<CollisionShape3D::SHAPE_MAX; ++j)
			{
				if(!collisionAlgorithmBuilderMatrix[i][j])
				{
					deleteCollisionAlgorithmBuilderMatrix();
					throw std::invalid_argument("Collision algorithm builder not initialized for shape type: " + std::to_string(i) + " and " + std::to_string(j) + ".");
				}
			}
		}

		//initialize algorithm pool
		unsigned int maxElementSize = 0;
		for(unsigned int i=0; i<CollisionShape3D::SHAPE_MAX; ++i)
		{
			for(unsigned int j=0; j<CollisionShape3D::SHAPE_MAX; ++j)
			{
				maxElementSize = std::max(maxElementSize, collisionAlgorithmBuilderMatrix[i][j]->getAlgorithmSize());
			}
		}
		unsigned int algorithmPoolSize = ConfigService::instance()->getUnsignedIntValue("narrowPhase.algorithmPoolSize");
		algorithmPool = new FixedSizePool<CollisionAlgorithm>(maxElementSize, algorithmPoolSize);
	}

	CollisionAlgorithmSelector::~CollisionAlgorithmSelector()
	{
		delete algorithmPool;

		deleteCollisionAlgorithmBuilderMatrix();
	}

	void CollisionAlgorithmSelector::initializeCollisionAlgorithmBuilderMatrix()
	{
		//initialize matrix with nullptr collision algorithm builder
		for(unsigned int i=0; i<CollisionShape3D::SHAPE_MAX; ++i)
		{
			for(unsigned int j=0; j<CollisionShape3D::SHAPE_MAX; ++j)
			{
				collisionAlgorithmBuilderMatrix[i][j] = nullptr;
			}
		}

		//specific algorithms
		collisionAlgorithmBuilderMatrix[CollisionShape3D::SPHERE_SHAPE][CollisionShape3D::SPHERE_SHAPE] = new SphereSphereCollisionAlgorithm::Builder();
		collisionAlgorithmBuilderMatrix[CollisionShape3D::SPHERE_SHAPE][CollisionShape3D::BOX_SHAPE] = new SphereBoxCollisionAlgorithm::Builder();
		collisionAlgorithmBuilderMatrix[CollisionShape3D::BOX_SHAPE][CollisionShape3D::SPHERE_SHAPE] = new SphereBoxCollisionAlgorithm::Builder();
		initializeConcaveAlgorithm();
		initializeCompoundAlgorithm();

		//generic algorithm (convex hull vs. convex hull)
		for(unsigned int i=0; i<CollisionShape3D::SHAPE_MAX; ++i)
		{
			for(unsigned int j=0; j<CollisionShape3D::SHAPE_MAX; ++j)
			{
				if(!collisionAlgorithmBuilderMatrix[i][j])
				{
					collisionAlgorithmBuilderMatrix[i][j] = new ConvexConvexCollisionAlgorithm::Builder();
				}
			}
		}
	}

	void CollisionAlgorithmSelector::initializeConcaveAlgorithm()
	{
        //heightfield shape
		for(unsigned int shapeId=0; shapeId<CollisionShape3D::SHAPE_MAX; ++shapeId)
		{
			collisionAlgorithmBuilderMatrix[CollisionShape3D::HEIGHTFIELD_SHAPE][shapeId] = new ConcaveAnyCollisionAlgorithm::Builder();

			if(shapeId!=CollisionShape3D::HEIGHTFIELD_SHAPE)
			{
				collisionAlgorithmBuilderMatrix[shapeId][CollisionShape3D::HEIGHTFIELD_SHAPE] = new ConcaveAnyCollisionAlgorithm::Builder();
			}
		}
	}

	void CollisionAlgorithmSelector::initializeCompoundAlgorithm()
	{
		for(unsigned int shapeId=0; shapeId<CollisionShape3D::SHAPE_MAX; ++shapeId)
		{
			if(!collisionAlgorithmBuilderMatrix[CollisionShape3D::COMPOUND_SHAPE][shapeId])
			{
				collisionAlgorithmBuilderMatrix[CollisionShape3D::COMPOUND_SHAPE][shapeId] = new CompoundAnyCollisionAlgorithm::Builder();
			}

			if(shapeId!=CollisionShape3D::COMPOUND_SHAPE && !collisionAlgorithmBuilderMatrix[shapeId][CollisionShape3D::COMPOUND_SHAPE])
			{
				collisionAlgorithmBuilderMatrix[shapeId][CollisionShape3D::COMPOUND_SHAPE] = new CompoundAnyCollisionAlgorithm::Builder();
			}
		}
	}

	void CollisionAlgorithmSelector::deleteCollisionAlgorithmBuilderMatrix()
	{
		for(unsigned int i=0; i<CollisionShape3D::SHAPE_MAX; ++i)
		{
			for(unsigned int j=0; j<CollisionShape3D::SHAPE_MAX; ++j)
			{
				delete collisionAlgorithmBuilderMatrix[i][j];
			}
		}
	}

	/**
	 * @param shape1 Shape or partial shape composing the body 1
	 * @param shape2 Shape or partial shape composing the body 2
	 */
	std::shared_ptr<CollisionAlgorithm> CollisionAlgorithmSelector::createCollisionAlgorithm(
			AbstractWorkBody *body1, const CollisionShape3D *shape1, AbstractWorkBody *body2, const CollisionShape3D *shape2)
	{
		CollisionAlgorithmBuilder *collisionAlgorithmBuilder = collisionAlgorithmBuilderMatrix[shape1->getShapeType()][shape2->getShapeType()];
        std::set<CollisionShape3D::ShapeType> firstExpectedShapeType = collisionAlgorithmBuilder->getFirstExpectedShapeType();

		void *memPtr = algorithmPool->allocate(collisionAlgorithmBuilder->getAlgorithmSize());
		if(firstExpectedShapeType.find(shape1->getShapeType())!=firstExpectedShapeType.end())
		{
			return std::shared_ptr<CollisionAlgorithm>(collisionAlgorithmBuilder->createCollisionAlgorithm(
					false, ManifoldResult(body1, body2), memPtr), AlgorithmDeleter(algorithmPool));
		}else if(firstExpectedShapeType.find(shape2->getShapeType())!=firstExpectedShapeType.end())
		{ //objects must be swap to match algorithm shape types
			return std::shared_ptr<CollisionAlgorithm>(collisionAlgorithmBuilder->createCollisionAlgorithm(
					true, ManifoldResult(body2, body1), memPtr), AlgorithmDeleter(algorithmPool));
		}else
		{
			throw std::runtime_error("Impossible to initialize collision algorithm for shape types: " + std::to_string(shape1->getShapeType())
									 + " and " + std::to_string(shape2->getShapeType()));
		}
	}

	CollisionAlgorithmSelector::AlgorithmDeleter::AlgorithmDeleter(FixedSizePool<CollisionAlgorithm> *const algorithmPool) :
		algorithmPool(algorithmPool)
	{

	}

	/**
	* Deleter for std::shared_ptr
	*/
	void CollisionAlgorithmSelector::AlgorithmDeleter::operator()(CollisionAlgorithm *const collisionAlgorithm)
	{
		algorithmPool->free(collisionAlgorithm);
	}

}
