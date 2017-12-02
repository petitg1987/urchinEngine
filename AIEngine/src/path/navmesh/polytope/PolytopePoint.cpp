#include "PolytopePoint.h"

namespace urchin
{
    PolytopePoint::PolytopePoint(const Point3<float> &point) :
            point(point)
    {
        faceIndices.reserve(3); //estimated memory size
    }

    PolytopePoint::PolytopePoint(const Point3<float> &point, const std::vector<unsigned int> &faceIndices) :
            point(point),
            faceIndices(faceIndices)
    {

    }

    void PolytopePoint::setPoint(const Point3<float> &point)
    {
        this->point = point;
    }

    const Point3<float> &PolytopePoint::getPoint() const
    {
        return point;
    }

    void PolytopePoint::addFaceIndex(unsigned int index)
    {
        faceIndices.push_back(index);
    }

    const std::vector<unsigned int> &PolytopePoint::getFaceIndices() const
    {
        return faceIndices;
    }

}
