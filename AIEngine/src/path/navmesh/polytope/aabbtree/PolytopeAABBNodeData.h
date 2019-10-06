#ifndef URCHINENGINE_POLYTOPEAABBNODEDATA_H
#define URCHINENGINE_POLYTOPEAABBNODEDATA_H

#include "UrchinCommon.h"

#include "path/navmesh/polytope/Polytope.h"

namespace urchin
{

    class PolytopeAABBNodeData : public AABBNodeData<Polytope>
    {
        public:
            explicit PolytopeAABBNodeData(Polytope *);

            AABBNodeData<Polytope> *clone() const override;

            const std::string &getObjectId() const override;
            AABBox<float> retrieveObjectAABBox() const override;
            bool isObjectMoving() const override;
    };

}

#endif
