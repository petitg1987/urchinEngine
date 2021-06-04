#pragma once

#include <stdexcept>
#include <UrchinCommon.h>

#include <collision/narrowphase/algorithm/gjk/result/GJKResult.h>
#include <collision/narrowphase/algorithm/util/Simplex.h>

namespace urchin {

    template<class T> class GJKResultCollide : public GJKResult<T> {
        public:
            explicit GJKResultCollide(const Simplex<T>&);

            bool isValidResult() const override;

            bool isCollide() const override;
            T getSeparatingDistance() const override;
            const Point3<T>& getClosestPointA() const override;
            const Point3<T>& getClosestPointB() const override;

            const Simplex<T>& getSimplex() const override;

        private:
            Simplex<T> simplex;
    };

}
