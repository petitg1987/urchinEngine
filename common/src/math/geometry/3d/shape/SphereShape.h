#ifndef ENGINE_SPHERESHAPE_H
#define ENGINE_SPHERESHAPE_H

#include "math/geometry/3d/shape/ConvexShape3D.h"

namespace urchin
{

	template<class T> class SphereShape : public ConvexShape3D
	{
		public:
			SphereShape(T);

			T getRadius() const;

		private:
			T radius;
	};

}

 #endif
