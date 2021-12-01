#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <resources/model/ConstMeshes.h>
#include <scene/renderer3d/model/Mesh.h>

namespace urchin {

    class Meshes {
        public:
            explicit Meshes(std::shared_ptr<ConstMeshes>);

            static constexpr float MIN_BBOX_SIZE = 0.01f;

            unsigned int getNumberMeshes() const;
            Mesh& getMesh(unsigned int) const;
            const AABBox<float>& getMeshesAABBox() const;
            const std::vector<AABBox<float>>& getMeshesSplitAABBoxes() const;
            const AABBox<float>& getLocalMeshesAABBox() const;

            const ConstMeshes& getConstMeshes() const;
            std::shared_ptr<ConstMeshes> copyConstMeshesRef() const;

            void updateMesh(unsigned int, const std::vector<Point3<float>>&);
            void updateMaterial(unsigned int, const std::shared_ptr<Material>&);

            void onMoving(const Transform<float>&);

        private:
            void computeLocalAABBox(bool);

            std::shared_ptr<ConstMeshes> constMeshes;
            unsigned int numMeshes;

            std::vector<std::unique_ptr<Mesh>> meshes;
            std::unique_ptr<AABBox<float>> localMeshesBBox;
            std::vector<AABBox<float>> localSplitBBoxes;
            AABBox<float> meshesBBox;
            std::vector<AABBox<float>> meshesSplitBBoxes;
    };

}
