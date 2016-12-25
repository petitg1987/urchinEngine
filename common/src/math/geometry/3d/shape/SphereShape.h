#ifndef ENGINE_SPHERESHAPE_H
#define ENGINE_SPHERESHAPE_H

#include <memory>

#include "math/geometry/3d/shape/ConvexShape3D.h"

namespace urchin
{

	template<class T> class SphereShape : public ConvexShape3D<T>
	{
		public:
			SphereShape(T);

			T getRadius() const;

			std::unique_ptr<ConvexObject3D<T>> toConvexObject(const Transform<T> &) const;

		private:
			T radius;
	};

}

 #endif
