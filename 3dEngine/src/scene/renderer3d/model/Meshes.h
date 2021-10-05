#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <resources/model/ConstMeshes.h>
#include <scene/renderer3d/model/Mesh.h>

namespace urchin {

    class Meshes {
        public:
            explicit Meshes(std::shared_ptr<ConstMeshes>);

            unsigned int getNumberMeshes() const;
            Mesh& getMesh(unsigned int) const;
            const AABBox<float>& getGlobalAABBox() const;
            const std::vector<AABBox<float>>& getGlobalSplitAABBoxes() const;
            const AABBox<float>& getGlobalLocalAABBox() const;

            const ConstMeshes& getConstMeshes() const;
            std::shared_ptr<ConstMeshes> copyConstMeshesRef() const;

            void updateMesh(unsigned int, const std::vector<Point3<float>>&);
            void updateMaterial(unsigned int, const std::shared_ptr<Material>&);

            void onMoving(const Transform<float>&);

        private:
            void computeLocalAABBox(bool);

            static constexpr float MIN_BBOX_SIZE = 0.01f;

            std::shared_ptr<ConstMeshes> constMeshes;
            unsigned int numMeshes;

            std::vector<std::unique_ptr<Mesh>> meshes;
            std::unique_ptr<AABBox<float>> localBBox; //bounding box (not transformed)
            std::vector<AABBox<float>> localSplitBBoxes;
            AABBox<float> globalBBox; //bounding box transformed by the transformation of the model
            std::vector<AABBox<float>> globalSplitBBoxes;
    };

}
