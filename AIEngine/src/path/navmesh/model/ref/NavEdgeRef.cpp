#include <cassert>

#include "NavEdgeRef.h"

namespace urchin
{
    NavEdgeRef::NavEdgeRef(const NavTriangleRef &triangleRef, unsigned int edgeIndex) :
        triangleRef(triangleRef),
        edgeIndex(edgeIndex)
    {
        #ifdef _DEBUG
            assert(edgeIndex <= 2);
        #endif
    }

    const NavTriangleRef &NavEdgeRef::getTriangleRef() const
    {
        return triangleRef;
    }

    unsigned int NavEdgeRef::getEdgeIndex() const
    {
        return edgeIndex;
    }

}
