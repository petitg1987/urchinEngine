#ifndef ENGINE_CONESHAPE_H
#define ENGINE_CONESHAPE_H

#include <memory>

#include "math/geometry/3d/shape/ConvexShape3D.h"

namespace urchin
{

	template<class T> class ConeShape : public ConvexShape3D<T>
	{
		public:
			enum ConeOrientation
			{
				CONE_X,
				CONE_Y,
				CONE_Z
			};

			ConeShape(T, T, ConeOrientation);

			T getRadius() const;
			T getHeight() const;
			ConeOrientation getConeOrientation() const;

			std::unique_ptr<ConvexObject3D<T>> toConvexObject(const Transform<T> &) const;

		private:
			T radius;
			T height;
			ConeOrientation coneOrientation;
	};

}

#endif
