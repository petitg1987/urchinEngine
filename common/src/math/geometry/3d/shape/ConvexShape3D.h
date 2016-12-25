#ifndef ENGINE_CONVEXSHAPE3D_H
#define ENGINE_CONVEXSHAPE3D_H

#include <memory>

#include "math/geometry/3d/object/ConvexObject3D.h"
#include "math/algebra/Transform.h"

namespace urchin
{

	/**
	* Represents a convex shape.
	* A shape doesn't have a position in space.
	*/
	template<class T> class ConvexShape3D
	{
		public:
			ConvexShape3D();
			virtual ~ConvexShape3D();

			virtual std::unique_ptr<ConvexObject3D<T>> toConvexObject(const Transform<T> &) const = 0;
	};

}

#endif
