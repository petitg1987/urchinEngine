#ifndef URCHINENGINE_TRIANGLESMODEL_H
#define URCHINENGINE_TRIANGLESMODEL_H

#include <vector>
#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class TrianglesModel : public GeometryModel
	{
		public:
			TrianglesModel(std::vector<Point3<float>> , std::vector<IndexedTriangle3D<float>> );

		protected:
			Matrix4<float> retrieveModelMatrix() const override;
			std::vector<Point3<float>> retrieveVertexArray() const override;

			void drawGeometry() const override;

		private:
			std::vector<Point3<float>> points;
			std::vector<IndexedTriangle3D<float>> triangles;
	};

}

#endif
