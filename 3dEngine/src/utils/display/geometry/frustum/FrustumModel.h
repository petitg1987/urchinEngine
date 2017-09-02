#ifndef URCHINENGINE_FRUSTUMMODEL_H
#define URCHINENGINE_FRUSTUMMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class FrustumModel : public GeometryModel
	{
		public:
			FrustumModel(const Frustum<float> &);
			~FrustumModel();

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			Frustum<float> frustum;
	};

}

#endif
