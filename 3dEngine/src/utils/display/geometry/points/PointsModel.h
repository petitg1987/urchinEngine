#ifndef ENGINE_POINTSMODEL_H
#define ENGINE_POINTSMODEL_H

#include <vector>
#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class PointsModel : public GeometryModel
	{
		public:
			PointsModel(const std::vector<Point3<float>> &, int);
			~PointsModel();

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			std::vector<Point3<float>> points;
			int pointsSize;
	};

}

#endif
