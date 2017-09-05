#ifndef URCHINENGINE_LINEMODEL_H
#define URCHINENGINE_LINEMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class LineModel : public GeometryModel
	{
		public:
			LineModel(const Line3D<float> &, int);

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			Line3D<float> line;
			int lineSize;
	};

}

#endif
