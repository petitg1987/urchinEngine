#ifndef URCHINENGINE_COLLISIONALGORITHM_H
#define URCHINENGINE_COLLISIONALGORITHM_H

#include "collision/ManifoldResult.h"
#include "collision/narrowphase/CollisionObjectWrapper.h"

namespace urchin {

    class CollisionAlgorithmSelector;

    class CollisionAlgorithm {
        public:
            CollisionAlgorithm(bool, ManifoldResult&&);
            virtual ~CollisionAlgorithm() = default;

            void setupCollisionAlgorithmSelector(const CollisionAlgorithmSelector*);

            void processCollisionAlgorithm(const CollisionObjectWrapper&, const CollisionObjectWrapper&, bool);

            bool isObjectSwapped() const;
            const ManifoldResult& getConstManifoldResult() const;

        protected:
            virtual void doProcessCollisionAlgorithm(const CollisionObjectWrapper&, const CollisionObjectWrapper&) = 0;

            const CollisionAlgorithmSelector* getCollisionAlgorithmSelector() const;

            ManifoldResult& getManifoldResult();
            void addNewContactPoint(const Vector3<float>&, const Point3<float>&, float);
            float getContactBreakingThreshold() const;

        private:
            void refreshContactPoints();

            bool objectSwapped;
            ManifoldResult manifoldResult;

            const CollisionAlgorithmSelector* collisionAlgorithmSelector;
    };

}

#endif
