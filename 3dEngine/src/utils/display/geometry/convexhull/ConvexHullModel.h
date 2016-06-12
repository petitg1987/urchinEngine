#ifndef ENGINE_CONVEXHULLMODEL_H
#define ENGINE_CONVEXHULLMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class ConvexHullModel : public GeometryModel
	{
		public:
			ConvexHullModel(const ConvexHull3D<float> &);
			~ConvexHullModel();

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			ConvexHull3D<float> convexHull;
	};

}

#endif
