#include <collision/narrowphase/algorithm/gjk/GJKResult.h>

namespace urchin {

    template<class T> GJKResult<T>::GJKResult(bool validResult, T separatingDistance, const Simplex<T>& simplex) :
            validResult(validResult),
            separatingDistance(separatingDistance),
            simplex(simplex) {
        if (isValidResult() && !isCollide()) {
            simplex.computeClosestPoints(closestPointA, closestPointB);

            if (DebugCheck::additionalChecksEnable()) {
                const T distanceDelta = closestPointA.vector(closestPointB).length() - separatingDistance;
                if (!MathFunction::isZero((float)distanceDelta, 0.01f)) {
                    logInputData("Incoherent separating distance (" + std::to_string(separatingDistance) + ") with simplex", simplex);
                }
            }
        }
    }

    template<class T> GJKResult<T> GJKResult<T>::newInvalidResult() {
        return GJKResult<T>(false, -1.0, Simplex<T>());
    }

    template<class T> GJKResult<T> GJKResult<T>::newCollideResult(const Simplex<T>& simplex) {
        return GJKResult<T>(true, -1.0, simplex);
    }

    template<class T> GJKResult<T> GJKResult<T>::newNoCollideResult(T separatingDistance, const Simplex<T>& simplex) {
        return GJKResult<T>(true, separatingDistance, simplex);
    }

    template<class T> bool GJKResult<T>::isValidResult() const {
        return validResult;
    }

    template<class T> bool GJKResult<T>::isCollide() const {
        #ifdef URCHIN_DEBUG
            assert(isValidResult());
        #endif
        return separatingDistance < 0.0f;
    }

    /**
     * @return Separating distance between convex hull (always positive)
     */
    template<class T> T GJKResult<T>::getSeparatingDistance() const {
        #ifdef URCHIN_DEBUG
            assert(!isCollide());
        #endif
        return separatingDistance;
    }

    /**
     * @return Closest point on object A in case of non collision
     */
    template<class T> const Point3<T>& GJKResult<T>::getClosestPointA() const {
        #ifdef URCHIN_DEBUG
            assert(!isCollide());
        #endif
        return closestPointA;
    }

    /**
     * @return Closest point on object B in case of non collision
     */
    template<class T> const Point3<T>& GJKResult<T>::getClosestPointB() const {
        #ifdef URCHIN_DEBUG
            assert(!isCollide());
        #endif
        return closestPointB;
    }

    template<class T> const Simplex<T>& GJKResult<T>::getSimplex() const {
        #ifdef URCHIN_DEBUG
            assert(isValidResult());
        #endif
        return simplex;
    }

    template<class T> void GJKResult<T>::logInputData(std::string_view errorMessage, const Simplex<T>& simplex) {
        std::stringstream logStream;
        logStream.precision(std::numeric_limits<T>::max_digits10);

        logStream << errorMessage << std::endl;
        logStream << simplex;

        Logger::instance().logError(logStream.str());
    }

    //explicit template
    template class GJKResult<float>;
    template class GJKResult<double>;

}
