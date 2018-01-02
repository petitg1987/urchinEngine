#ifndef URCHINENGINE_AABBOXMODEL_H
#define URCHINENGINE_AABBOXMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class AABBoxModel : public GeometryModel
	{
		public:
			explicit AABBoxModel(const AABBox<float> &);

		protected:
			Matrix4<float> retrieveModelMatrix() const override;
			std::vector<Point3<float>> retrieveVertexArray() const override;

			void drawGeometry() const override;

		private:
			AABBox<float> aabbox;
	};

}

#endif
