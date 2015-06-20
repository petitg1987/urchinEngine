#ifndef ENGINE_SPHERESHAPE_H
#define ENGINE_SPHERESHAPE_H

#include "Shape3D.h"

namespace urchin
{

	template<class T> class SphereShape : public Shape3D
	{
		public:
			SphereShape(T);

			T getRadius() const;

		private:
			T radius;
	};

}

 #endif
