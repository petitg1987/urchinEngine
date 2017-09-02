#ifndef URCHINENGINE_COLLISIONALGORITHM_H
#define URCHINENGINE_COLLISIONALGORITHM_H

#include "collision/ManifoldResult.h"
#include "collision/narrowphase/CollisionObjectWrapper.h"

namespace urchin
{

	class CollisionAlgorithm
	{
		public:
			CollisionAlgorithm(bool, const ManifoldResult &);
			virtual ~CollisionAlgorithm();

			void processCollisionAlgorithm(const CollisionObjectWrapper &, const CollisionObjectWrapper &, bool);

			const ManifoldResult &getConstManifoldResult() const;
			bool isObjectSwapped() const;

		protected:
			virtual void doProcessCollisionAlgorithm(const CollisionObjectWrapper &, const CollisionObjectWrapper &) = 0;

			ManifoldResult &getManifoldResult();
			void addNewContactPoint(const Vector3<float> &, const Point3<float> &, float);

			float getContactBreakingThreshold() const;

		private:
			void refreshContactPoints();

			bool objectSwapped;
			ManifoldResult manifoldResult;
	};

}

#endif
