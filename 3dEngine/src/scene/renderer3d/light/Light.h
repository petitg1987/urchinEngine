#ifndef ENGINE_LIGHT_H
#define ENGINE_LIGHT_H

#include <vector>
#include "UrchinCommon.h"

#include "scene/renderer3d//octree/Octreeable.h"

namespace urchin
{

	class Light : public Observable, public Octreeable<Light>
	{
		public:
			Light();
			virtual ~Light();

			enum LightType
			{
				OMNIDIRECTIONAL,
				SUN
			};

			enum NotificationType
			{
				MOVE, //A light has been moved (position, direction...)
				PRODUCE_SHADOW, //Produce shadow flag has been updated
			};

			//general methods
			virtual std::vector<Vector3<float>> getDirections() const = 0;
			virtual const Point3<float> &getPosition() const = 0;
			virtual bool hasParallelBeams() const = 0;
			virtual LightType getLightType() const =0;
			virtual const AABBox<float> &getAABBox() const = 0;
			const Transform<float> &getTransform() const;

			//shadow methods
			void setProduceShadow(bool);
			bool isProduceShadow() const;

			//properties methods
			void setAmbientColor(const Point3<float> &);
			const Point3<float> &getAmbientColor() const;

		private:
			//general properties
			Point3<float> ambientColor;

			//shadow properties
			bool produceShadow;
	};

}

#endif
