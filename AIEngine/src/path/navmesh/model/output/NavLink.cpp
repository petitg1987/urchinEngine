#include "NavLink.h"
#include "path/navmesh/model/output/NavPolygon.h"
#include "path/navmesh/model/output/NavTriangle.h"

namespace urchin
{
    NavLink::NavLink(LinkType linkType, const std::shared_ptr<NavPolygon> &targetPolygon, const std::shared_ptr<NavTriangle> &targetTriangle) :
            linkType(linkType),
            targetPolygon(targetPolygon),
            targetTriangle(targetTriangle)
    { //TODO add edge limitation for jump

    }


    NavLink::LinkType NavLink::getLinkType() const
    {
        return linkType;
    }

    std::shared_ptr<NavPolygon> NavLink::getTargetPolygon() const
    {
        return targetPolygon.lock();
    }

    std::shared_ptr<NavTriangle> NavLink::getTargetTriangle() const
    {
        return targetTriangle.lock();
    }


}
