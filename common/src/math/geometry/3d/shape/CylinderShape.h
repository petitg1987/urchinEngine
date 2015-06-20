#ifndef ENGINE_CYLINDERSHAPE_H
#define ENGINE_CYLINDERSHAPE_H

#include "Shape3D.h"

namespace urchin
{

	template<class T> class CylinderShape : public Shape3D
	{
		public:
			enum CylinderOrientation
			{
				CYLINDER_X,
				CYLINDER_Y,
				CYLINDER_Z
			};

			CylinderShape(T, T, CylinderOrientation);

			T getRadius() const;
			T getHeight() const;
			CylinderOrientation getCylinderOrientation() const;

		private:
			T radius;
			T height;
			CylinderOrientation cylinderOrientation;
	};

}

 #endif
