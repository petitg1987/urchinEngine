#pragma once

#include <stdexcept>
#include <UrchinCommon.h>

#include <collision/narrowphase/algorithm/gjk/result/GJKResult.h>
#include <collision/narrowphase/algorithm/util/Simplex.h>

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
            void logInputData(std::string_view, const Simplex<T>&);

            T separatingDistance;
            Point3<T> closestPointA;
            Point3<T> closestPointB;

            Simplex<T> simplex;
    };

}
