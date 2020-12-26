#include "GJKResultNoCollide.h"

namespace urchin {

    /**
     * @param separatingDistance Separating distance of two objects. In case of collision, the distance is zero.
     */
    template<class T> GJKResultNoCollide<T>::GJKResultNoCollide(T separatingDistance, const Simplex<T>& simplex) :
        GJKResult<T>(),
        separatingDistance(separatingDistance),
        simplex(simplex) {
        simplex.computeClosestPoints(closestPointA, closestPointB);

        if (DebugCheck::instance()->additionalChecksEnable()) {
            const T distanceDelta = closestPointA.vector(closestPointB).length() - separatingDistance;
            if (!MathFunction::instance()->isZero((float)distanceDelta, 0.01f)) {
                logInputData("Incoherent separating distance (" + std::to_string(separatingDistance) + ") with simplex", simplex);
            }
        }
    }

    template<class T> bool GJKResultNoCollide<T>::isValidResult() const {
        return true;
    }

    template<class T> bool GJKResultNoCollide<T>::isCollide() const {
        return false;
    }

    /**
     * @return Separating distance between convex hull (always positive)
     */
    template<class T> T GJKResultNoCollide<T>::getSeparatingDistance() const {
        return separatingDistance;
    }

    template<class T> const Simplex<T> &GJKResultNoCollide<T>::getSimplex() const {
        return simplex;
    }

    /**
     * @return Closest point on object A in case of non collision
     */
    template<class T> const Point3<T> &GJKResultNoCollide<T>::getClosestPointA() const {
        return closestPointA;
    }

    /**
     * @return Closest point on object B in case of non collision
     */
    template<class T> const Point3<T> &GJKResultNoCollide<T>::getClosestPointB() const {
        return closestPointB;
    }

    template<class T> void GJKResultNoCollide<T>::logInputData(const std::string& errorMessage, const Simplex<T>& simplex) {
        std::stringstream logStream;
        logStream.precision(std::numeric_limits<T>::max_digits10);

        logStream << errorMessage << std::endl;
        logStream << simplex;

        Logger::instance()->logError(logStream.str());
    }

    //explicit template
    template class GJKResultNoCollide<float>;
    template class GJKResultNoCollide<double>;

}
