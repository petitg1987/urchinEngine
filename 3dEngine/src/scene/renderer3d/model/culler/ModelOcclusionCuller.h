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
            template<class FILTER> void getModelsInOBBox(const OBBox<float>&, std::vector<Model*>&, const FILTER&) const;

            std::unique_ptr<AABBoxModel> createDebugGeometries();

            void refresh();
            void postRefresh();

        private:
            OctreeManager<Model> modelOctreeManager;
    };

    #include "ModelOcclusionCuller.inl"

}
