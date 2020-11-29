#ifndef URCHINENGINE_AABBNODEDATA_H
#define URCHINENGINE_AABBNODEDATA_H

#include <string>

#include "math/geometry/3d/object/AABBox.h"

namespace urchin {

    template<class OBJ> class AABBNodeData {
        public:
            explicit AABBNodeData(OBJ);
            virtual ~AABBNodeData() = default;

            OBJ getNodeObject() const;

            virtual AABBNodeData<OBJ>* clone() const = 0;

            virtual const std::string& getObjectId() const = 0;
            virtual AABBox<float> retrieveObjectAABBox() const = 0;
            virtual bool isObjectMoving() const = 0;

        private:
            OBJ nodeObject;
    };

    #include "AABBNodeData.inl"

}

#endif
