#ifndef ENGINE_COLLISIONCONVEXHULLREADERWRITER_H
#define ENGINE_COLLISIONCONVEXHULLREADERWRITER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "CollisionShapeReaderWriter.h"

namespace urchin
{

	class CollisionConvexHullReaderWriter : public CollisionShapeReaderWriter
	{
		//XML tags
		#define POINTS_TAG "points"
		#define POINT_TAG "point"

		public:
			CollisionConvexHullReaderWriter();
			virtual ~CollisionConvexHullReaderWriter();

			CollisionShape3D *loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writeOn(std::shared_ptr<XmlChunk>, const CollisionShape3D *, XmlWriter &) const;
	};

}

#endif
