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
			TrianglesModel(const std::vector<Point3<float>> &, const std::vector<IndexedTriangle3D<float>> &);
			~TrianglesModel();

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			std::vector<Point3<float>> points;
			std::vector<IndexedTriangle3D<float>> triangles;
	};

}

#endif
