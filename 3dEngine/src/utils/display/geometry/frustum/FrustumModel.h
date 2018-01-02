#ifndef URCHINENGINE_FRUSTUMMODEL_H
#define URCHINENGINE_FRUSTUMMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class FrustumModel : public GeometryModel
	{
		public:
			explicit FrustumModel(const Frustum<float> &);

		protected:
			Matrix4<float> retrieveModelMatrix() const override;
			std::vector<Point3<float>> retrieveVertexArray() const override;

			void drawGeometry() const override;

		private:
			Frustum<float> frustum;
	};

}

#endif
