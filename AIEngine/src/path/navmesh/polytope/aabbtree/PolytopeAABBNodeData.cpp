#include "PolytopeAABBNodeData.h"

namespace urchin
{

    PolytopeAABBNodeData::PolytopeAABBNodeData(const std::shared_ptr<Polytope> &polytope) :
            AABBNodeData(polytope)
    {

    }

    AABBNodeData<std::shared_ptr<Polytope>> *PolytopeAABBNodeData::clone() const
    {
        throw std::logic_error("Clone not required for static polytope");
    }

    const std::string &PolytopeAABBNodeData::getObjectId() const
    {
        return getNodeObject()->getName();
    }

    AABBox<float> PolytopeAABBNodeData::retrieveObjectAABBox() const
    {
        return getNodeObject()->getAABBox();
    }

    bool PolytopeAABBNodeData::isObjectMoving() const
    {
        return false; //never move: always deleted and recreated
    }

}
