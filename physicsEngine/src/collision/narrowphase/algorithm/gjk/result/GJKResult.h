#pragma once

#include <UrchinCommon.h>

#include <collision/narrowphase/algorithm/util/Simplex.h>
#include <collision/narrowphase/algorithm/util/AlgorithmResult.h>

namespace urchin {

    template<class T> class GJKResult : public AlgorithmResult {
        public:
            ~GJKResult() override = default;

            virtual bool isValidResult() const = 0;

            virtual bool isCollide() const = 0;
            virtual T getSeparatingDistance() const = 0;
            virtual const Point3<T>& getClosestPointA() const = 0;
            virtual const Point3<T>& getClosestPointB() const = 0;

            virtual const Simplex<T>& getSimplex() const = 0;
    };

}
