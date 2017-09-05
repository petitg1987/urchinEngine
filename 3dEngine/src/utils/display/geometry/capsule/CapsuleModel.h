#ifndef URCHINENGINE_CAPSULEMODEL_H
#define URCHINENGINE_CAPSULEMODEL_H

#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

	class CapsuleModel : public GeometryModel
	{
		public:
			CapsuleModel(const Capsule<float> &, int, int);

		protected:
			Matrix4<float> retrieveModelMatrix() const;
			std::vector<Point3<float>> retrieveVertexArray() const;

			void drawGeometry() const;

		private:
			void fillWithTopCap(std::vector<Point3<float>> &, const Quaternion<float> &) const;
			void fillWithBottomCap(std::vector<Point3<float>> &, const Quaternion<float> &) const;

			Capsule<float> capsule;
			int sides, slices;
	};

}

#endif
