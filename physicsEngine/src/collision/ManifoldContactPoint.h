#pragma once

#include <UrchinCommon.h>

#include <collision/constraintsolver/solvingdata/AccumulatedSolvingData.h>

namespace urchin {

    class ManifoldContactPoint {
        public:
            ManifoldContactPoint();
            ManifoldContactPoint(const Vector3<float>&, const Point3<float>&, const Point3<float>&, const Point3<float>&, const Point3<float>&, float, bool);

            const Vector3<float>& getNormalFromObject2() const;
            const Point3<float>& getPointOnObject1() const;
            const Point3<float>& getPointOnObject2() const;
            const Point3<float>& getLocalPointOnObject1() const;
            const Point3<float>& getLocalPointOnObject2() const;
            float getDepth() const;
            bool isPredictive() const;

            void updatePoints(const Point3<float>&, const Point3<float>&);
            void updateDepth(float);

            AccumulatedSolvingData& getAccumulatedSolvingData();

        private:
            Vector3<float> normalFromObject2;
            Point3<float> pointOnObject1, pointOnObject2;
            Point3<float> localPointOnObject1, localPointOnObject2;
            float depth;
            bool bIsPredictive;

            AccumulatedSolvingData accumulatedSolvingData;
    };

}
