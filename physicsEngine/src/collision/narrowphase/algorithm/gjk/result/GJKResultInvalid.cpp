#include "GJKResultInvalid.h"

namespace urchin {

    template<class T> bool GJKResultInvalid<T>::isValidResult() const {
        return false;
    }

    template<class T> bool GJKResultInvalid<T>::isCollide() const {
        throw std::domain_error("Invalid GJK result: cannot call this method.");
    }

    template<class T> T GJKResultInvalid<T>::getSeparatingDistance() const {
        throw std::domain_error("Invalid GJK result: cannot call this method.");
    }

    template<class T> const Simplex<T> &GJKResultInvalid<T>::getSimplex() const {
        throw std::domain_error("Invalid GJK result: cannot call this method.");
    }

    template<class T> const Point3<T> &GJKResultInvalid<T>::getClosestPointA() const {
        throw std::domain_error("Invalid GJK result: cannot call this method.");
    }

    template<class T> const Point3<T> &GJKResultInvalid<T>::getClosestPointB() const {
        throw std::domain_error("Invalid GJK result: cannot call this method.");
    }

    //explicit template
    template class GJKResultInvalid<float>;
    template class GJKResultInvalid<double>;

}
