#ifndef ENGINE_COLLISIONSHAPEREADERWRITERRETRIEVER_H
#define ENGINE_COLLISIONSHAPEREADERWRITERRETRIEVER_H

#include <memory>

#include "CollisionShapeReaderWriter.h"
#include "UrchinPhysicsEngine.h"

namespace urchin
{

	/**
	* Retrieve strategy to read/write collision shape
	*/
	class CollisionShapeReaderWriterRetriever
	{
		public:
			static std::shared_ptr<CollisionShapeReaderWriter> retrieveShapeReaderWriter(std::shared_ptr<XmlChunk>);
			static std::shared_ptr<CollisionShapeReaderWriter> retrieveShapeReaderWriter(const CollisionShape3D *);
	};

}

#endif
