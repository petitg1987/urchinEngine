#pragma once

#include <scene/renderer3d/model/Model.h>
#include <resources/geometry/aabbox/AABBoxModel.h>

namespace urchin {

    class ModelOcclusionCuller {
        public:
            ModelOcclusionCuller();

            void addModel(std::shared_ptr<Model>);
            std::shared_ptr<Model> removeModel(Model*);

            void getModelsInFrustum(const Frustum<float>&, std::vector<Model*>&) const;

            std::unique_ptr<AABBoxModel> createDebugGeometries();

            void refresh();
            void postRefresh();

        private:
            OctreeManager<Model> modelOctreeManager;
    };

}
