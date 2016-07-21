#ifndef ENGINE_RAYTESTER_H
#define ENGINE_RAYTESTER_H

#include "UrchinCommon.h"

#include "processable/Processable.h"
#include "processable/raytest/RayTestCallback.h"
#include "collision/CollisionWorld.h"

namespace urchin
{

	class RayTester : public Processable
	{
		public:
			RayTester(const Ray<float> &ray, RayTestCallback &);
			~RayTester();

			void initialize(PhysicsWorld *);

			void setup(float, const Vector3<float> &);
			void execute(float, const Vector3<float> &);

		private:
			const Ray<float> ray;
			RayTestCallback &rayTestCallback;

			CollisionWorld *collisionWorld;

	};

}

#endif
