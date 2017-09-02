#ifndef URCHINENGINE_SUNLIGHT_H
#define URCHINENGINE_SUNLIGHT_H

#include "UrchinCommon.h"

#include "scene/renderer3d/light/Light.h"

namespace urchin
{

	class SunLight : public Light
	{
		public:
			SunLight(const Vector3<float> &);
			virtual ~SunLight();

			//general methods
			void setDirection(const Vector3<float> &);
			std::vector<Vector3<float>> getDirections() const;
			const Point3<float> &getPosition() const;
			LightType getLightType() const;
			bool hasParallelBeams() const;
			const AABBox<float> &getAABBox() const;

		private:
			Vector3<float> direction;
	};

}

#endif
