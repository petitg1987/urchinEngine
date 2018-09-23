#include "CSGPolygon.h"

namespace urchin
{

	template<class T> CSGPolygon<T>::CSGPolygon(std::string name, const std::vector<Point2<T>> &cwPoints) :
		name(std::move(name)),
		cwPoints(cwPoints)
	{
		#ifdef _DEBUG
            if(cwPoints.size()>=3)
            {
				//assert no duplicate points
                for (unsigned int i = 0; i < cwPoints.size(); ++i)
                {
                    for (unsigned int j = 0; j < cwPoints.size(); ++j)
                    {
                        assert(i == j || cwPoints[i].X != cwPoints[j].X || cwPoints[i].Y != cwPoints[j].Y);
                    }
                }

				//assert clockwise order
                double area = 0.0;
                for (unsigned int i = 0, prevI = cwPoints.size() - 1; i < cwPoints.size(); prevI=i++)
                {
                    area += (cwPoints[i].X - cwPoints[prevI].X) * (cwPoints[i].Y + cwPoints[prevI].Y);
                }
               	assert(area >= 0.0);
            }
    	#endif
	}

	template<class T> CSGPolygon<T>::CSGPolygon(const CSGPolygon &polygon) :
		name(polygon.name),
		cwPoints(polygon.cwPoints)
	{

	}

	template<class T> CSGPolygon<T>::CSGPolygon(CSGPolygon &&polygon) noexcept :
		name(std::move(polygon.name)),
		cwPoints(std::move(polygon.cwPoints))
	{

	}

	template<class T> CSGPolygon<T>& CSGPolygon<T>::operator=(CSGPolygon<T> &&polygon) noexcept
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

		auto area = (T)0;
	    for (unsigned int i=1; i<=n; i++)
	    {
	        area -= cwPoints[i].X * (cwPoints[i+1].Y - cwPoints[i-1].Y);
	    }

		cwPoints.pop_back();
	    cwPoints.pop_back();

	    return area / (T)2;
	}

	template<class T> bool CSGPolygon<T>::pointInsidePolygon(const Point2<T> &point) const
	{
		return pointInsidePolygon(point, false);
	}

	template<class T> bool CSGPolygon<T>::pointInsideOrOnPolygon(const Point2<T> &point) const
	{
		return pointInsidePolygon(point, true);
	}

	template<class T> bool CSGPolygon<T>::pointInsidePolygon(const Point2<T> &point, bool onEdgeIsInside) const
	{//see http://web.archive.org/web/20120323102807/http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
		bool inside = false;

		for(unsigned int i=0, previousI=cwPoints.size()-1; i<cwPoints.size(); previousI=i++)
		{
			Point2<T> point1 = cwPoints[previousI];
			Point2<T> point2 = cwPoints[i];

			if (((point1.Y<=point.Y && point.Y<point2.Y) || (point2.Y<=point.Y && point.Y<point1.Y))
				//same but without division: ((point.X-point1.X) < (point2.X-point1.X) * (point.Y-point1.Y) / (point2.Y-point1.Y))
				&& ((point.X-point1.X)*std::abs(point2.Y-point1.Y) < (point2.X-point1.X) * (point.Y-point1.Y) * MathAlgorithm::sign<T>(point2.Y-point1.Y)))
			{
				inside = !inside;
			}else if(onEdgeIsInside && LineSegment2D<T>(point1, point2).onSegment(point))
			{
				return true;
			}
		}

		return inside;
	}

	template<class T> CSGPolygon<T> CSGPolygon<T>::expand(T distance) const
	{
		return CSGPolygon<T>(name, ResizePolygon2DService<T>::instance()->resizePolygon(cwPoints, -distance));
	}

	template<class T> CSGPolygon<T> CSGPolygon<T>::simplify(T polygonMinDotProductThreshold, T polygonMergePointsDistanceThreshold) const
	{
		std::vector<Point2<T>> simplifiedCwPoints;
		simplifiedCwPoints.reserve(cwPoints.size());

		const T mergePointsSquareDistance = polygonMergePointsDistanceThreshold * polygonMergePointsDistanceThreshold;
		for(unsigned int i=0, previousI=cwPoints.size()-1; i<cwPoints.size(); previousI=i++)
		{
            if(simplifiedCwPoints.empty() || cwPoints[i].squareDistance(simplifiedCwPoints[simplifiedCwPoints.size()-1]) > mergePointsSquareDistance) //exclude points too close
            {
                unsigned int nextI = (i+1) % cwPoints.size();

                Vector2<T> normalizedEdge1 = cwPoints[previousI].vector(cwPoints[i]).normalize();
                Vector2<T> normalizedEdge2 = cwPoints[i].vector(cwPoints[nextI]).normalize();
                T absDotProduct = std::abs(normalizedEdge1.dotProduct(normalizedEdge2));

                if(absDotProduct < polygonMinDotProductThreshold) //exclude angles near to 180 and 0/360 degrees
                {
                    simplifiedCwPoints.emplace_back(cwPoints[i]);
                }
            }
		}

		if(simplifiedCwPoints.size() > 2 && simplifiedCwPoints[0].squareDistance(simplifiedCwPoints[simplifiedCwPoints.size()-1]) <= mergePointsSquareDistance)
		{ //first point and last point are too close: remove last one
			simplifiedCwPoints.resize(simplifiedCwPoints.size()-1);
		}

		if(simplifiedCwPoints.size() < 3)
		{
			return CSGPolygon<T>("[[null]]", std::vector<Point2<T>>());
		}

		return CSGPolygon<T>(name, simplifiedCwPoints);
	}

	template<class T> bool CSGPolygon<T>::isSelfIntersect() const
	{
		for(unsigned int i = 0; i<cwPoints.size(); ++i)
		{
			LineSegment2D<T> line1(cwPoints[i], cwPoints[(i+1) % cwPoints.size()]);

			for(unsigned int j = i+2; j<cwPoints.size() - 1; ++j)
			{
				LineSegment2D<T> line2(cwPoints[j], cwPoints[(j+1) % cwPoints.size()]);

				bool hasIntersection;
				line1.intersectPoint(line2, hasIntersection);
				if(hasIntersection)
				{
					return true;
				}
			}
		}

		return false;
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const CSGPolygon<T> &polygon)
	{
		stream << "Name: " << polygon.getName() << std::endl;
		stream << "Points (CW): " << std::endl;
		for(const auto &point : polygon.getCwPoints())
		{
			stream << "\t" << point << std::endl;
		}
		return stream;
	}

	//explicit template
	template class CSGPolygon<float>;
	template std::ostream& operator <<<float>(std::ostream &, const CSGPolygon<float> &);

	template class CSGPolygon<long long>;
	template std::ostream& operator <<<long long>(std::ostream &, const CSGPolygon<long long> &);
}
