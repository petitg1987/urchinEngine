#ifndef URCHINENGINE_COLLISIONCYLINDERREADERWRITER_H
#define URCHINENGINE_COLLISIONCYLINDERREADERWRITER_H

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
			~CollisionCylinderReaderWriter() override = default;

			CollisionShape3D *loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const override;
			void writeOn(std::shared_ptr<XmlChunk>, const CollisionShape3D *, XmlWriter &) const override;
	};

}

#endif
