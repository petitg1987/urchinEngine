#ifndef ENGINE_COLLISIONSPHEREREADERWRITER_H
#define ENGINE_COLLISIONSPHEREREADERWRITER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "CollisionShapeReaderWriter.h"

namespace urchin
{

	class CollisionSphereReaderWriter : public CollisionShapeReaderWriter
	{
		//XML tags
		#define RADIUS_TAG "radius"

		public:
			CollisionSphereReaderWriter();
			virtual ~CollisionSphereReaderWriter();

			CollisionShape3D *loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writeOn(std::shared_ptr<XmlChunk>, const CollisionShape3D *, XmlWriter &) const;
	};

}

#endif
