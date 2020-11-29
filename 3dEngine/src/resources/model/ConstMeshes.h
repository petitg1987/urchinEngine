#ifndef URCHINENGINE_CONSTMESHES_H
#define URCHINENGINE_CONSTMESHES_H

#include <vector>
#include <string>
#include "UrchinCommon.h"

#include "resources/model/ConstMesh.h"
#include "resources/Resource.h"

namespace urchin {

    /**
     * Contains all the constant/common data for meshes.
     * Two identical models can use the instance of this class.
     */
    class ConstMeshes : public Resource {
        public:
            ConstMeshes(std::string , const std::vector<const ConstMesh *>&);
            ~ConstMeshes() override;

            const std::string &getMeshFilename() const;
            unsigned int getNumberConstMeshes() const;
            const ConstMesh *getConstMesh(unsigned int) const;
            const std::vector<const ConstMesh *> &getConstMeshes() const;

            const AABBox<float> &getOriginalAABBox() const;
            const std::vector<AABBox<float>> &getOriginalSplitAABBoxes() const;

        private:
            std::string meshFilename;
            std::vector<const ConstMesh *> constMeshes;

            AABBox<float> *originalBBox; //bounding box (not transformed)
            std::vector<AABBox<float>> originalSplitBBoxes;
    };

}

#endif
