#include "CSGPolygon.h"

namespace urchin
{

	template<class T> CSGPolygon<T>::CSGPolygon(std::string name, const std::vector<Point2<T>> &cwPoints) :
		name(std::move(name)),
		cwPoints(cwPoints)
	{

	}

	template<class T> CSGPolygon<T>::CSGPolygon(const CSGPolygon &polygon) :
		name(polygon.name),
		cwPoints(polygon.cwPoints)
	{

	}

	template<class T> CSGPolygon<T>::CSGPolygon(CSGPolygon &&polygon) :
		name(std::move(polygon.name)),
		cwPoints(std::move(polygon.cwPoints))
	{

	}

	template<class T> CSGPolygon<T>& CSGPolygon<T>::operator=(CSGPolygon<T> &&polygon)
	{
		this->name = std::move(polygon.name);
		this->cwPoints = std::move(polygon.cwPoints);
		return *this;
	}

	template<class T> const std::string &CSGPolygon<T>::getName() const
	{
		return name;
	}

	template<class T> const std::vector<Point2<T>> &CSGPolygon<T>::getCwPoints() const
	{
		return cwPoints;
	}

	template<class T> T CSGPolygon<T>::computeArea() const
	{
		unsigned int n = cwPoints.size();
		cwPoints.reserve(n+2);
		cwPoints.push_back(cwPoints[0]);
		cwPoints.push_back(cwPoints[1]);

		T area = (T)0;
	    for (unsigned int i=1; i<=n; i++)
	    {
	        area -= cwPoints[i].X * (cwPoints[i+1].Y - cwPoints[i-1].Y);
	    }

		cwPoints.pop_back();
	    cwPoints.pop_back();

	    return area / (T)2;
	}

	template<class T> CSGPolygon<T> CSGPolygon<T>::expand(T distance) const
	{
		return CSGPolygon<T>(name, ResizePolygon2DService<T>::instance()->resizePolygon(cwPoints, -distance));
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const CSGPolygon<T> &polygon)
	{
		stream << "Name:" << polygon.getName() << std::endl;
		stream << "Points (CW):" << std::endl;
		for(auto point : polygon.getCwPoints())
		{
			stream << point << std::endl;
		}
		return stream;
	}

	//explicit template
	template class CSGPolygon<float>;
	template std::ostream& operator <<<float>(std::ostream &, const CSGPolygon<float> &);

	template class CSGPolygon<double>;
	template std::ostream& operator <<<double>(std::ostream &, const CSGPolygon<double> &);

	template class CSGPolygon<int>;
	template std::ostream& operator <<<int>(std::ostream &, const CSGPolygon<int> &);

}
