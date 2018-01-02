#ifndef URCHINENGINE_SPHEREMODEL_H
#define URCHINENGINE_SPHEREMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class SphereModel : public GeometryModel
	{
		public:
			SphereModel(const Sphere<float> &, int);

		protected:
			Matrix4<float> retrieveModelMatrix() const override;
			std::vector<Point3<float>> retrieveVertexArray() const override;

			void drawGeometry() const override;

		private:
			Sphere<float> sphere;
			int slices;
	};

}

#endif
