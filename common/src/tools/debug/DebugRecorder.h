#ifndef ENGINE_DEBUGRECORDER_H
#define ENGINE_DEBUGRECORDER_H

#include <string>
#include <fstream>

#include "math/algebra/point/Point3.h"
#include "math/geometry/3d/object/ConvexHull3D.h"

namespace urchin
{

	/**
	* Debug recorder can be setup on some physics process in order to record debug data. The record produced can be then used in a
	* debug visualizer to replay the process step by step.
	*/
	template<class T> class DebugRecorder
	{
		#define POINT3_ACTION "POINT3"
		#define CONVEX_HULL_3D_ACTION "CONVEX_HULL_3D"
		#define CLEAR_ENTITY_ACTION "CLEAR_ENTITY"

		#define NUM_INDEXED_TRIANGLES "NumIndexedTriangles"
		#define NUM_INDEXED_POINTS "NumIndexedPoints"

		public:
			DebugRecorder(const std::string &);
			~DebugRecorder();

			void recordPoint(const std::string &, const Point3<T> &);
			void recordConvexHull(const std::string &, const ConvexHull3D<T> &);

			void clearEntity(const std::string &);

		private:
			std::ofstream file;

			void writeAction(const std::string &);
	};

}

#endif
