#ifndef ENGINE_CONVEXSHAPE3D_H
#define ENGINE_CONVEXSHAPE3D_H

namespace urchin
{

	/**
	* Represents a convex shape.
	* A shape doesn't have a position in space.
	*/
	class ConvexShape3D
	{
		public:
			ConvexShape3D();
			virtual ~ConvexShape3D();
	};

}

#endif
