#ifndef URCHINENGINE_POLYTOPEAABBNODEDATA_H
#define URCHINENGINE_POLYTOPEAABBNODEDATA_H

#include <UrchinCommon.h>
#include <path/navmesh/model/NavObject.h>

namespace urchin {

    class NavObjectAABBNodeData : public AABBNodeData<std::shared_ptr<NavObject>> {
        public:
            explicit NavObjectAABBNodeData(const std::shared_ptr<NavObject>&);

            AABBNodeData<std::shared_ptr<NavObject>>* clone() const override;

            const std::string& getObjectId() const override;
            AABBox<float> retrieveObjectAABBox() const override;
            bool isObjectMoving() const override;
    };

}

#endif
