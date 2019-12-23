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
	        explicit TrianglesModel(std::vector<Point3<float>>);

		protected:
			Matrix4<float> retrieveModelMatrix() const override;
			std::vector<Point3<float>> retrieveVertexArray() const override;

			void drawGeometry() const override;

		private:
			std::vector<Point3<float>> trianglesPoints;
	};

}

#endif
