#ifndef URCHINENGINE_CONEMODEL_H
#define URCHINENGINE_CONEMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class ConeModel : public GeometryModel
	{
		public:
			ConeModel(const Cone<float> &, int);
			~ConeModel();

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			Cone<float> cone;
			int slices;
	};

}

#endif
