#ifndef URCHINENGINE_MANIFOLDRESULT_H
#define URCHINENGINE_MANIFOLDRESULT_H

#include "UrchinCommon.h"

#include "collision/ManifoldContactPoint.h"
#include "body/work/AbstractWorkBody.h"

#define MAX_PERSISTENT_POINTS 4

namespace urchin {

    /**
    * Persistent result of a collision between two shapes
    */
    class ManifoldResult {
        public:
            ManifoldResult(AbstractWorkBody *, AbstractWorkBody*);
            ManifoldResult(const ManifoldResult&) = default;
            ManifoldResult(ManifoldResult &&) = default;

            AbstractWorkBody *getBody(unsigned int) const;
            AbstractWorkBody *getBody1() const;
            AbstractWorkBody *getBody2() const;

            unsigned int getNumContactPoints() const;
            float getContactBreakingThreshold() const;
            ManifoldContactPoint &getManifoldContactPoint(unsigned int);
            const ManifoldContactPoint &getManifoldContactPoint(unsigned int) const;

            void addContactPoint(const Vector3<float>&, const Point3<float>&, float, bool);
            void addContactPoint(const Vector3<float>&, const Point3<float>&, const Point3<float>&, const Point3<float>&, const Point3<float>&, float, bool);
            void refreshContactPoints();

        private:
            int getNearestPointIndex(const Point3<float>&) const;
            unsigned int computeBestInsertionIndex(const Point3<float>&) const;
            unsigned int getDeepestPointIndex() const;
            float computeArea(const Point3<float>&, const Point3<float>&, const Point3<float>&, const Point3<float>&) const;
            void removeContactPoint(unsigned int);

            AbstractWorkBody *body1;
            AbstractWorkBody *body2;

            ManifoldContactPoint contactPoints[MAX_PERSISTENT_POINTS];
            unsigned int nbContactPoint;
            const float contactBreakingThreshold;
    };

}

#endif
