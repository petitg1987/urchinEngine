#ifndef URCHINENGINE_CONVEXHULLMODEL_H
#define URCHINENGINE_CONVEXHULLMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class ConvexHullModel : public GeometryModel
	{
		public:
			ConvexHullModel(const ConvexHull3D<float> &);

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			ConvexHull3D<float> convexHull;
	};

}

#endif
