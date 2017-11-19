#include "PolyhedronPoint.h"

namespace urchin
{
    PolyhedronPoint::PolyhedronPoint(const Point3<float> &point) :
            point(point)
    {
        faceIndices.reserve(3); //estimated memory size
    }

    PolyhedronPoint::PolyhedronPoint(const Point3<float> &point, const std::vector<unsigned int> &faceIndices) :
            point(point),
            faceIndices(faceIndices)
    {

    }

    void PolyhedronPoint::setPoint(const Point3<float> &point)
    {
        this->point = point;
    }

    const Point3<float> &PolyhedronPoint::getPoint() const
    {
        return point;
    }

    void PolyhedronPoint::addFaceIndex(unsigned int index)
    {
        faceIndices.push_back(index);
    }

    const std::vector<unsigned int> &PolyhedronPoint::getFaceIndices() const
    {
        return faceIndices;
    }

}
