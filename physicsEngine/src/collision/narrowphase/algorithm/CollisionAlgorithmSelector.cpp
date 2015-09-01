#include <stdexcept>
#include <fstream>

#include "collision/narrowphase/algorithm/CollisionAlgorithmSelector.h"
#include "collision/narrowphase/algorithm/SphereSphereCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/SphereBoxCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/ConvexHullConvexHullCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CompoundCollisionAlgorithm.h"

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
				if(collisionAlgorithmBuilderMatrix[i][j]==nullptr)
				{
					deleteCollisionAlgorithmBuilderMatrix();

					std::ostringstream buffer;
					buffer<<"Collision algorithm builder not initialized for shape type: "<<i<<" and "<<j<<".";
					throw std::invalid_argument(buffer.str());
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
		for(unsigned int shapeId=0; shapeId<CollisionShape3D::SHAPE_MAX; ++shapeId)
		{
			collisionAlgorithmBuilderMatrix[CollisionShape3D::COMPOUND_SHAPE][shapeId] = new CompoundCollisionAlgorithm::Builder();
			if(shapeId!=CollisionShape3D::COMPOUND_SHAPE)
			{
				collisionAlgorithmBuilderMatrix[shapeId][CollisionShape3D::COMPOUND_SHAPE] = new CompoundCollisionAlgorithm::Builder();
			}
		}

		//generic algorithm (convex hull vs. convex hull)
		for(unsigned int i=0; i<CollisionShape3D::SHAPE_MAX; ++i)
		{
			for(unsigned int j=0; j<CollisionShape3D::SHAPE_MAX; ++j)
			{
				if(collisionAlgorithmBuilderMatrix[i][j]==nullptr)
				{
					collisionAlgorithmBuilderMatrix[i][j] = new ConvexHullConvexHullCollisionAlgorithm::Builder();
				}
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
		#ifdef _DEBUG
			assert(collisionAlgorithmBuilder->getShapeType1()!=CollisionShape3D::ANY_TYPE);
		#endif

		void *memPtr = algorithmPool->allocate(collisionAlgorithmBuilder->getAlgorithmSize());

		if(collisionAlgorithmBuilder->getShapeType1()==shape1->getShapeType())
		{
			return std::shared_ptr<CollisionAlgorithm>(collisionAlgorithmBuilder->createCollisionAlgorithm(
					false, ManifoldResult(body1, body2), memPtr), AlgorithmDeleter(algorithmPool));
		}else
		{
			//objects must be swap to match algorithm shape types
			return std::shared_ptr<CollisionAlgorithm>(collisionAlgorithmBuilder->createCollisionAlgorithm(
					true, ManifoldResult(body2, body1), memPtr), AlgorithmDeleter(algorithmPool));
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
