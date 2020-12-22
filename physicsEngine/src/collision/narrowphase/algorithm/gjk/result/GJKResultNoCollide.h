#ifndef URCHINENGINE_GJKRESULTNOCOLLIDE_H
#define URCHINENGINE_GJKRESULTNOCOLLIDE_H

#include <stdexcept>
#include "UrchinCommon.h"

#include "GJKResult.h"
#include "collision/narrowphase/algorithm/util/Simplex.h"

namespace urchin {

    template<class T> class GJKResultNoCollide : public GJKResult<T> {
        public:
            GJKResultNoCollide(T, const Simplex<T>&);

            bool isValidResult() const override;

            bool isCollide() const override;
            T getSeparatingDistance() const override;
            const Point3<T>& getClosestPointA() const override;
            const Point3<T>& getClosestPointB() const override;

            const Simplex<T>& getSimplex() const override;

        private:
            void logInputData(const std::string&, const Simplex<T>&);

            T separatingDistance;
            Point3<T> closestPointA, closestPointB;

            Simplex<T> simplex;
    };

}

#endif
