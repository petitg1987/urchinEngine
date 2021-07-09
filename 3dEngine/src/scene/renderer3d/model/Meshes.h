#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <resources/model/ConstMeshes.h>
#include <scene/renderer3d/model/Mesh.h>

namespace urchin {

    class Meshes {
        public:
            explicit Meshes(ConstMeshes*);
            ~Meshes();

            unsigned int getNumberMeshes() const;
            Mesh& getMesh(unsigned int) const;
            const AABBox<float>& getGlobalAABBox() const;
            const std::vector<AABBox<float>>& getGlobalSplitAABBoxes() const;
            const AABBox<float>& getGlobalLocalAABBox() const;

            const ConstMeshes* getConstMeshes() const;

            void onMoving(const Transform<float>&);

        private:
            mutable ConstMeshes* constMeshes;
            unsigned int numMeshes;

            std::vector<std::unique_ptr<Mesh>> meshes;
            AABBox<float> globalBBox; //bounding box transformed by the transformation of the model
            std::vector<AABBox<float>> globalSplitBBoxes;
    };

}
