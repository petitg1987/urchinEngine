#pragma once

#include <scene/renderer3d/model/Model.h>
#include <resources/geometry/aabbox/AABBoxModel.h>

namespace urchin {

    class ModelOcclusionCuller : public Observer {
        public:
            ModelOcclusionCuller();

            void notify(Observable*, int) override;

            void addModel(std::shared_ptr<Model>);
            std::shared_ptr<Model> removeModel(Model*);

            void getModelsInFrustum(const Frustum<float>&, std::vector<Model*>&) const;
            template<class FILTER> void getModelsInOBBox(const OBBox<float>&, std::vector<Model*>&, const FILTER&) const;
            std::vector<std::shared_ptr<Model>> getAllModels() const;

            std::unique_ptr<AABBoxModel> createDebugGeometries();

            void refresh();
            void postRefresh();

        private:
            std::shared_ptr<Model> removeModel(Model*, Model::CullBehavior);

            template<class FILTER> void addNoCullModels(std::vector<Model*>&, const FILTER&) const;

            OctreeManager<Model> modelOctreeManager;
            std::vector<std::shared_ptr<Model>> noCullModels;
    };

    #include "ModelOcclusionCuller.inl"

}
