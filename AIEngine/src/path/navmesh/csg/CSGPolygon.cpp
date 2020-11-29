#include "CSGPolygon.h"

namespace urchin {

    template<class T> CSGPolygon<T>::CSGPolygon(std::string name, const std::vector<Point2<T>>& cwPoints) :
            name(std::move(name)),
            cwPoints(cwPoints) {
        #ifndef NDEBUG
            assert(isCwPoints());
        #endif
    }

    template<class T> CSGPolygon<T>::CSGPolygon(std::string name, std::vector<Point2<T>> &&cwPoints) :
            name(std::move(name)),
            cwPoints(std::move(cwPoints)) {
        #ifndef NDEBUG
            assert(isCwPoints());
        #endif
    }

    template<class T> CSGPolygon<T>::CSGPolygon(const CSGPolygon& polygon) :
        name(polygon.name),
        cwPoints(polygon.cwPoints) {

    }

    template<class T> CSGPolygon<T>::CSGPolygon(CSGPolygon&& polygon) noexcept :
        name(std::move(polygon.name)),
        cwPoints(std::move(polygon.cwPoints)) {

    }

    template<class T> CSGPolygon<T>& CSGPolygon<T>::operator=(CSGPolygon<T>&& polygon) noexcept {
        this->name = std::move(polygon.name);
        this->cwPoints = std::move(polygon.cwPoints);
        return *this;
    }

    template<class T> const std::string &CSGPolygon<T>::getName() const {
        return name;
    }

    template<class T> const std::vector<Point2<T>> &CSGPolygon<T>::getCwPoints() const {
        return cwPoints;
    }

    template<class T> T CSGPolygon<T>::computeArea() const {
        std::size_t n = cwPoints.size();
        cwPoints.reserve(n + 2);
        cwPoints.push_back(cwPoints[0]);
        cwPoints.push_back(cwPoints[1]);

        auto area = (T)0;
        for (std::size_t i = 1; i <= n; i++) {
            area -= cwPoints[i].X * (cwPoints[i + 1].Y - cwPoints[i - 1].Y);
        }

        cwPoints.pop_back();
        cwPoints.pop_back();

        return area / (T)2;
    }

    template<class T> bool CSGPolygon<T>::pointInsidePolygon(const Point2<T>& point) const {
        return pointInsidePolygon(point, false);
    }

    template<class T> bool CSGPolygon<T>::pointInsideOrOnPolygon(const Point2<T>& point) const {
        return pointInsidePolygon(point, true);
    }

    template<class T> bool CSGPolygon<T>::pointInsidePolygon(const Point2<T>& point, bool onEdgeIsInside) const {//see http://web.archive.org/web/20120323102807/http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
        bool inside = false;

        for (std::size_t i=0, previousI=cwPoints.size()-1; i<cwPoints.size(); previousI=i++) {
            Point2<T> point1 = cwPoints[previousI];
            Point2<T> point2 = cwPoints[i];

            if (((point1.Y<=point.Y && point.Y<point2.Y) || (point2.Y<=point.Y && point.Y<point1.Y))
                //same but without division: ((point.X-point1.X) < (point2.X-point1.X) * (point.Y-point1.Y) / (point2.Y-point1.Y))
                && ((point.X-point1.X)*std::abs(point2.Y-point1.Y) < (point2.X-point1.X) * (point.Y-point1.Y) * MathAlgorithm::sign<T>(point2.Y-point1.Y))) {
                inside = !inside;
            } else if (onEdgeIsInside && LineSegment2D<T>(point1, point2).onSegment(point)) {
                return true;
            }
        }

        return inside;
    }

    template<class T> void CSGPolygon<T>::expand(T distance) {
        std::unique_ptr<CSGPolygon<T>> originalPolygon(nullptr);
        if (Check::instance()->additionalChecksEnable()) {
            originalPolygon = std::make_unique<CSGPolygon<T>>(*this);
        }

        ResizePolygon2DService<T>::instance()->resizePolygon(cwPoints, -distance);

        if (Check::instance()->additionalChecksEnable() && !isCwPoints()) {
            logInputData("Impossible to expand polygon (distance: " + std::to_string(distance) + ")", Logger::ERROR, *originalPolygon);
        }
    }

    template<class T> void CSGPolygon<T>::simplify(T polygonMinDotProductThreshold, T polygonMergePointsDistanceThreshold) {
        std::unique_ptr<CSGPolygon<T>> originalPolygon(nullptr);
        if (Check::instance()->additionalChecksEnable()) {
            originalPolygon = std::make_unique<CSGPolygon<T>>(*this);
        }

        const T mergePointsSquareDistance = polygonMergePointsDistanceThreshold * polygonMergePointsDistanceThreshold;

        //exclude points too close (only neighbor points)
        for (int i=0; i<(int)cwPoints.size(); i++) {
            int nextI = (i+1) % cwPoints.size();
            if (cwPoints[i].squareDistance(cwPoints[nextI]) <= mergePointsSquareDistance) {
                cwPoints.erase(cwPoints.begin() + nextI);
                i--;
                continue;
            }
        }

        //exclude angles near to 180 and 0/360 degrees
        for (int i=0; i<(int)cwPoints.size(); i++) {
            int nextI = (i+1) % cwPoints.size();
            int previousI = (i==0) ? cwPoints.size()-1 : i-1;

            Vector2<T> normalizedEdge1 = cwPoints[previousI].vector(cwPoints[i]).normalize();
            Vector2<T> normalizedEdge2 = cwPoints[i].vector(cwPoints[nextI]).normalize();
            T absDotProduct = std::abs(normalizedEdge1.dotProduct(normalizedEdge2));
            if (absDotProduct >= polygonMinDotProductThreshold) {
                cwPoints.erase(cwPoints.begin() + i);
                i--;
                continue;
            }
        }

        //move or exclude points too close (not neighbor points)
        for (int i=0; i<(int)cwPoints.size(); i++) {
            //Use j=i+3 because:
            // cwPoints[i] and cwPoints[i+1] cannot be close points: they should be already erased by first loop because they are close neighbor
            // cwPoints[i] and cwPoints[i+2] cannot be close points: they should be already erased by previous loop because of the angle between cwPoints[i]-cwPoints[i+1]-cwPoints[i+2]
            bool keepGoing = true;
            for (int j=i+3; j<(int)cwPoints.size() && keepGoing; j++) {
                if (cwPoints[i].squareDistance(cwPoints[j]) <= mergePointsSquareDistance) {
                    int previousI = (i==0) ? cwPoints.size()-1 : i-1;
                    Vector2<T> moveVector = cwPoints[i].vector(cwPoints[previousI]);
                    T moveVectorLength = moveVector.length();
                    if (moveVectorLength > 2 * polygonMergePointsDistanceThreshold) {
                        cwPoints[i] = cwPoints[i].translate((moveVector / moveVectorLength) * polygonMergePointsDistanceThreshold);
                    } else {
                        cwPoints.erase(cwPoints.begin() + i);
                        i--;
                        keepGoing = false;
                    }
                }
            }
        }

        if (cwPoints.size() < 3) {
            cwPoints.clear();
        }

        if (Check::instance()->additionalChecksEnable() && !isCwPoints()) {
            logInputData("Impossible to simplify polygon (dot: " + std::to_string(polygonMinDotProductThreshold) + ", distance: " + std::to_string(polygonMergePointsDistanceThreshold) + ")", Logger::ERROR, *originalPolygon);
        }
    }

    template<class T> bool CSGPolygon<T>::isCwPoints() const {
        if (cwPoints.size() >= 3) {
            //assert no duplicate points
            for (std::size_t i = 0; i < cwPoints.size(); ++i) {
                for (std::size_t j = 0; j < cwPoints.size(); ++j) {
                    if (i != j && cwPoints[i].X == cwPoints[j].X && cwPoints[i].Y == cwPoints[j].Y) {
                        return false;
                    }
                }
            }

            //assert clockwise order
            double area = 0.0;
            for (std::size_t i = 0, prevI = cwPoints.size() - 1; i < cwPoints.size(); prevI=i++) {
                area += (cwPoints[i].X - cwPoints[prevI].X) * (cwPoints[i].Y + cwPoints[prevI].Y);
            }
            if (area < 0.0) {
                return false;
            }

            //assert no lines intersection
            if (cwPoints.size() < 20) //don't check big polygons for performance reason
            {
                for (unsigned int i=0; i<cwPoints.size(); ++i) {
                    unsigned int iNext = (i+1)%cwPoints.size();
                    LineSegment2D<double> line1(cwPoints[i].template cast<double>(), cwPoints[iNext].template cast<double>());
                    for (unsigned int j=0; j<cwPoints.size(); ++j) {
                        unsigned int jNext = (j + 1) % cwPoints.size();
                        if (i != j && iNext != j && i != jNext) {
                            LineSegment2D<double> line2(cwPoints[j].template cast<double>(), cwPoints[jNext].template cast<double>());

                            bool hasIntersection;
                            line1.intersectPoint(line2, hasIntersection);
                            if (hasIntersection) {
                                return false;
                            }
                        }
                    }
                }
            }
        }

        return true;
    }

    template<class T> SVGPolygon *CSGPolygon<T>::toCsvPolygon(SVGShape::SVGColor color) const {
        std::vector<Point2<float>> cwPointsFloat;
        cwPointsFloat.reserve(cwPoints.size());
        for (const auto &point : cwPoints) {
            cwPointsFloat.emplace_back(point.template cast<float>());
        }

        auto *svgPolygon = new SVGPolygon(cwPointsFloat, color, 0.5f);
        svgPolygon->setStroke(color, 0.01f);
        return svgPolygon;
    }

    template<class T> void CSGPolygon<T>::logInputData(const std::string& message, Logger::CriticalityLevel logLevel, const CSGPolygon<T>& inputPolygon) const {
        std::stringstream logStream;
        logStream.precision(std::numeric_limits<float>::max_digits10);

        logStream<<message<<std::endl;
        logStream<<inputPolygon;

        Logger::logger().log(logLevel, logStream.str());
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const CSGPolygon<T>& polygon) {
        stream << "Name: " << polygon.getName() << std::endl;
        stream << "Points (CW): " << std::endl;
        for (const auto &point : polygon.getCwPoints()) {
            stream << "\t" << point << std::endl;
        }
        return stream;
    }

    //explicit template
    template class CSGPolygon<float>;
    template std::ostream& operator <<<float>(std::ostream&, const CSGPolygon<float>&);

    template class CSGPolygon<long long>;
    template std::ostream& operator <<<long long>(std::ostream&, const CSGPolygon<long long>&);
}
