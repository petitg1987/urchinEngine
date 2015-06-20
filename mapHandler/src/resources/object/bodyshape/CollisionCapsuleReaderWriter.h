#ifndef ENGINE_COLLISIONCAPSULEREADERWRITER_H
#define ENGINE_COLLISIONCAPSULEREADERWRITER_H

#include <memory>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "CollisionShapeReaderWriter.h"

namespace urchin
{

	class CollisionCapsuleReaderWriter : public CollisionShapeReaderWriter
	{
		//XML tags
		#define ORIENTATION_TAG "orientation"
		#define RADIUS_TAG "radius"
		#define CYLINDER_HEIGHT_TAG "cylinderHeight"

		//XML value
		#define X_VALUE "X"
		#define Y_VALUE "Y"
		#define Z_VALUE "Z"

		public:
			CollisionCapsuleReaderWriter();
			virtual ~CollisionCapsuleReaderWriter();

			CollisionShape3D *loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writeOn(std::shared_ptr<XmlChunk>, const CollisionShape3D *, XmlWriter &) const;
	};

}

#endif
