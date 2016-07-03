#ifndef ENGINE_BOXSHAPE_H
#define ENGINE_BOXSHAPE_H

#include <vector>

#include "Shape3D.h"
#include "math/algebra/vector/Vector3.h"
#include "math/algebra/matrix/Matrix3.h"

namespace urchin
{

	template<class T> class BoxShape : public Shape3D
	{
		public:
			BoxShape(const Vector3<T> &);
			~BoxShape();

			const T getHalfSize(unsigned int) const;
			const Vector3<T> &getHalfSizes() const;

			T getVolume() const;

		private:
			Vector3<T> halfSizes; //half sizes (width, height and depth) of box
	};

}

 #endif
