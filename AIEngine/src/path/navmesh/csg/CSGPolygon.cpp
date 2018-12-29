#include "CSGPolygon.h"

namespace urchin
{

	template<class T> CSGPolygon<T>::CSGPolygon(std::string name, const std::vector<Point2<T>> &cwPoints) :
			name(std::move(name)),
			cwPoints(cwPoints)
	{
		#ifdef _DEBUG
			checkCwPoints();
		#endif
	}

	template<class T> CSGPolygon<T>::CSGPolygon(std::string name, std::vector<Point2<T>> &&cwPoints) :
			name(std::move(name)),
			cwPoints(std::move(cwPoints))
	{
		#ifdef _DEBUG
			checkCwPoints();
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

	template<class T> void CSGPolygon<T>::checkCwPoints() const
	{
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
	{ //TODO update "this" ?
		return CSGPolygon<T>(name, ResizePolygon2DService<T>::instance()->resizePolygon(cwPoints, -distance));
	}

	template<class T> void CSGPolygon<T>::simplify(T polygonMinDotProductThreshold, T polygonMergePointsDistanceThreshold)
	{
        const T mergePointsSquareDistance = polygonMergePointsDistanceThreshold * polygonMergePointsDistanceThreshold;

        for(int i=0; i<cwPoints.size(); i++)
        {
            int nextI = (i+1) % cwPoints.size();
            if(cwPoints[i].squareDistance(cwPoints[nextI]) <= mergePointsSquareDistance)
            { //exclude points too close
                cwPoints.erase(cwPoints.begin()+nextI);
                i--;
                continue;
            }

            int previousI = (i==0) ? cwPoints.size()-1 : i-1;
            Vector2<T> normalizedEdge1 = cwPoints[previousI].vector(cwPoints[i]).normalize();
            Vector2<T> normalizedEdge2 = cwPoints[i].vector(cwPoints[nextI]).normalize();
            T absDotProduct = std::abs(normalizedEdge1.dotProduct(normalizedEdge2));
            if(absDotProduct >= polygonMinDotProductThreshold)
            { //exclude angles near to 180 and 0/360 degrees
                cwPoints.erase(cwPoints.begin()+i);
                i--;
                continue;
            }
        }

        if(cwPoints.size() > 2 && cwPoints[0].squareDistance(cwPoints.back()) <= mergePointsSquareDistance)
        { //first point and last point are too close: remove last one
            cwPoints.resize(cwPoints.size()-1);
        }

        if(cwPoints.size() < 3)
        {
            cwPoints.clear();
        }
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
