#include "collision/narrowphase/algorithm/epa/EPAResultInvalid.h"

namespace urchin
{

	template<class T> bool EPAResultInvalid<T>::isValidResult() const
	{
		return false;
	}

	template<class T> bool EPAResultInvalid<T>::isCollide() const
	{
		throw std::domain_error("Invalid EPA result: cannot call this method.");
	}

	template<class T> const Point3<T> &EPAResultInvalid<T>::getContactPointA() const
	{
		throw std::domain_error("Invalid EPA result: cannot call this method.");
	}

	template<class T> const Point3<T> &EPAResultInvalid<T>::getContactPointB() const
	{
		throw std::domain_error("Invalid EPA result: cannot call this method.");
	}

	template<class T> const Vector3<T> &EPAResultInvalid<T>::getNormal() const
	{
		throw std::domain_error("Invalid EPA result: cannot call this method.");
	}

	template<class T> T EPAResultInvalid<T>::getPenetrationDepth() const
	{
		throw std::domain_error("Invalid EPA result: cannot call this method.");
	}

	//explicit template
	template class EPAResultInvalid<float>;
	template class EPAResultInvalid<double>;

}
