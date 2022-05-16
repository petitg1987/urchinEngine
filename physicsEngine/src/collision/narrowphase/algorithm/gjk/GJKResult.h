#pragma once

#include <UrchinCommon.h>

#include <collision/narrowphase/algorithm/util/Simplex.h>

namespace urchin {

    template<class T> class GJKResult {
        public:
            static GJKResult<T> newInvalidResult();
            static GJKResult<T> newCollideResult(const Simplex<T>&);
            static GJKResult<T> newNoCollideResult(T, const Simplex<T>&);

            bool isValidResult() const;

            bool isCollide() const;
            T getSeparatingDistance() const;
            const Point3<T>& getClosestPointA() const;
            const Point3<T>& getClosestPointB() const;

            const Simplex<T>& getSimplex() const;

        private:
            GJKResult(bool, T, const Simplex<T>&);

            void logInputData(std::string_view, const Simplex<T>&);

            bool validResult;

            T separatingDistance;
            Point3<T> closestPointA;
            Point3<T> closestPointB;

            Simplex<T> simplex;
    };

}
