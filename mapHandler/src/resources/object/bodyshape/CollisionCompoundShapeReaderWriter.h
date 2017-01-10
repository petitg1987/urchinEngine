#ifndef ENGINE_COLLISIONCOMPOUNDSHAPEREADERWRITER_H
#define ENGINE_COLLISIONCOMPOUNDSHAPEREADERWRITER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "CollisionShapeReaderWriter.h"

namespace urchin
{

	class CollisionCompoundShapeReaderWriter : public CollisionShapeReaderWriter
	{
		//XML tags
		#define LOCALIZED_SHAPES "localizedShapes"
		#define LOCALIZED_SHAPE "localizedShape"
		#define TRANSFORM_TAG "transform"
		#define POSITION_TAG "position"
		#define SHAPE "shape"

		public:
			CollisionCompoundShapeReaderWriter();
			virtual ~CollisionCompoundShapeReaderWriter();

			CollisionShape3D *loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writeOn(std::shared_ptr<XmlChunk>, const CollisionShape3D *, XmlWriter &) const;

		private:
			void loadTransformOn(std::shared_ptr<LocalizedCollisionShape>, std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writeTransformOn(std::shared_ptr<XmlChunk>, std::shared_ptr<const LocalizedCollisionShape>, XmlWriter &) const;

	};

}

#endif
