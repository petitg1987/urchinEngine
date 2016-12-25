#ifndef ENGINE_CAPSULESHAPE_H
#define ENGINE_CAPSULESHAPE_H

#include "math/geometry/3d/shape/ConvexShape3D.h"

namespace urchin
{

	template<class T> class CapsuleShape : public ConvexShape3D
	{
		public:
			enum CapsuleOrientation
			{
				CAPSULE_X,
				CAPSULE_Y,
				CAPSULE_Z
			};

			CapsuleShape(T, T, CapsuleOrientation);

			T getRadius() const;
			T getCylinderHeight() const;
			CapsuleOrientation getCapsuleOrientation() const;

			T computeHeight() const;

		private:
			T radius;
			T cylinderHeight;
			CapsuleOrientation capsuleOrientation;
	};

}

 #endif
