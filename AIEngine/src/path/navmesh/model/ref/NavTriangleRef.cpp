#include "NavTriangleRef.h"

namespace urchin
{
    NavTriangleRef::NavTriangleRef(unsigned int polygonIndex, unsigned int triangleIndex) :
            polygonIndex(polygonIndex),
            triangleIndex(triangleIndex)
    {

    }

    unsigned int NavTriangleRef::getPolygonIndex() const
    {
        return polygonIndex;
    }

    unsigned int NavTriangleRef::getTriangleIndex() const
    {
        return triangleIndex;
    }

    bool NavTriangleRef::equals(const NavTriangleRef &other) const
    {
        return polygonIndex==other.getPolygonIndex() && triangleIndex==other.getTriangleIndex();
    }

}
