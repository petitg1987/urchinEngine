#ifndef ENGINE_COLLISIONCYLINDERREADERWRITER_H
#define ENGINE_COLLISIONCYLINDERREADERWRITER_H

#include <memory>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "CollisionShapeReaderWriter.h"

namespace urchin
{

	class CollisionCylinderReaderWriter : public CollisionShapeReaderWriter
	{
		//XML tags
		#define ORIENTATION_TAG "orientation"
		#define RADIUS_TAG "radius"
		#define HEIGHT_TAG "height"

		//XML value
		#define X_VALUE "X"
		#define Y_VALUE "Y"
		#define Z_VALUE "Z"

		public:
			CollisionCylinderReaderWriter();
			virtual ~CollisionCylinderReaderWriter();

			CollisionShape3D *loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writeOn(std::shared_ptr<XmlChunk>, const CollisionShape3D *, XmlWriter &) const;
	};

}

#endif
