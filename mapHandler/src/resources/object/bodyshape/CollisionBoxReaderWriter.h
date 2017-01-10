#ifndef ENGINE_COLLISIONBOXREADERWRITER_H
#define ENGINE_COLLISIONBOXREADERWRITER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "CollisionShapeReaderWriter.h"

namespace urchin
{

	class CollisionBoxReaderWriter : public CollisionShapeReaderWriter
	{
		//XML tags
		#define HALF_SIZE_TAG "halfSize"

		public:
			CollisionBoxReaderWriter();
			virtual ~CollisionBoxReaderWriter();

			CollisionShape3D *loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writeOn(std::shared_ptr<XmlChunk>, const CollisionShape3D *, XmlWriter &) const;
	};

}

#endif
