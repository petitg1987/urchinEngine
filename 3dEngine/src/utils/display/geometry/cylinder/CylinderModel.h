#ifndef ENGINE_CYLINDERMODEL_H
#define ENGINE_CYLINDERMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class CylinderModel : public GeometryModel
	{
		public:
			CylinderModel(const Cylinder<float> &, int);
			~CylinderModel();

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			Cylinder<float> cylinder;
			int sides;
	};

}

#endif
