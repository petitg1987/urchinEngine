#pragma once

#include <UrchinCommon.h>

#include <collision/ManifoldContactPoint.h>
#include <body/model/AbstractBody.h>

#define MAX_PERSISTENT_POINTS 4

namespace urchin {

    /**
    * Persistent result of a collision between two shapes
    */
    class ManifoldResult {
        public:
            ManifoldResult(AbstractBody*, AbstractBody*);
            ManifoldResult(const ManifoldResult&) = default;
            ManifoldResult(ManifoldResult&&) = default;

            AbstractBody* getBody(unsigned int) const;
            AbstractBody* getBody1() const;
            AbstractBody* getBody2() const;

            unsigned int getNumContactPoints() const;
            float getContactBreakingThreshold() const;
            ManifoldContactPoint& getManifoldContactPoint(unsigned int);
            const ManifoldContactPoint& getManifoldContactPoint(unsigned int) const;

            void addContactPoint(const Vector3<float>&, const Point3<float>&, float, bool);
            void addContactPoint(const Vector3<float>&, const Point3<float>&, const Point3<float>&, const Point3<float>&, const Point3<float>&, float, bool);
            void refreshContactPoints();

        private:
            int getNearestPointIndex(const Point3<float>&) const;
            unsigned int computeBestInsertionIndex(const Point3<float>&) const;
            unsigned int getDeepestPointIndex() const;
            float computeArea(const Point3<float>&, const Point3<float>&, const Point3<float>&, const Point3<float>&) const;
            void removeContactPoint(std::size_t);

            AbstractBody* body1;
            AbstractBody* body2;

            std::array<ManifoldContactPoint, MAX_PERSISTENT_POINTS> contactPoints;
            unsigned int nbContactPoint;
            const float contactBreakingThreshold;
    };

}
