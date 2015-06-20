#ifndef ENGINE_AABBOXMODEL_H
#define ENGINE_AABBOXMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class AABBoxModel : public GeometryModel
	{
		public:
			AABBoxModel(const AABBox<float> &);
			~AABBoxModel();

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			AABBox<float> aabbox;
	};

}

#endif
