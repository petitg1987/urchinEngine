#ifndef ENGINE_TRIANGLESMODEL_H
#define ENGINE_TRIANGLESMODEL_H

#include <vector>
#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class TrianglesModel : public GeometryModel
	{
		public:
			TrianglesModel(const std::vector<Point3<float>> &, const std::vector<IndexedTriangle2D<float>> &);
			~TrianglesModel();

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			std::vector<Point3<float>> points;
			std::vector<IndexedTriangle2D<float>> triangles;
	};

}

#endif
