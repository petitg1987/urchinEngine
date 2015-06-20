#ifndef ENGINE_COLLISIONCONVEXOBJECT3D_H
#define ENGINE_COLLISIONCONVEXOBJECT3D_H

#include "UrchinCommon.h"

namespace urchin
{

	class CollisionConvexObject3D
	{
		public:
			CollisionConvexObject3D(float);
			virtual ~CollisionConvexObject3D();

			float getOuterMargin() const;

			virtual Point3<float> getSupportPoint(const Vector3<float> &, bool) const = 0;

		private:
			const float outerMargin;
	};

}

#endif
