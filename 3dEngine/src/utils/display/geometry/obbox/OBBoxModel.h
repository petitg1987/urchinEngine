#ifndef URCHINENGINE_OBBOXMODEL_H
#define URCHINENGINE_OBBOXMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class OBBoxModel : public GeometryModel
	{
		public:
			OBBoxModel(const OBBox<float> &);
			~OBBoxModel();

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			OBBox<float> obbox;
	};

}

#endif
