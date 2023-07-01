#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <UrchinCommon.h>

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/displayer/DisplayMode.h>
#include <scene/renderer3d/model/displayer/ModelInstanceDisplayer.h>
#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>
#include <scene/renderer3d/camera/Camera.h>
#include <resources/geometry/GeometryContainer.h>

namespace urchin {

    /**
    * Allow to display models given in parameter depending of a configuration (view matrix, projection matrix, display mode...)
    */
    class ModelSetDisplayer final : public Observer {
        public:
            using ModelSortFunction = bool (*)(const Model*, const Model*);

            explicit ModelSetDisplayer(DisplayMode displayMode);
            ~ModelSetDisplayer() override;

            void initialize(RenderTarget&);
            void setupShader(std::string, std::string, std::string, std::unique_ptr<ShaderConstants>);
            void setupCustomShaderVariable(std::unique_ptr<CustomModelShaderVariable>);
            void setupDepthOperations(bool, bool);
            void setupFaceCull(bool);
            void setupDisplayModelsInOrder(bool);
            void setupBlendFunctions(const std::vector<BlendFunction>&);
            void setupMeshFilter(std::unique_ptr<MeshFilter>);

            void notify(Observable*, int) override;

            void updateModels(std::span<Model* const>);
            void removeModel(Model*);
            std::span<Model* const> getModels() const;
            bool isDisplayerExist(const Model&) const;

            void prepareRendering(unsigned int, const Matrix4<float>&);
            void prepareRendering(unsigned int&, const Matrix4<float>&, ModelSortFunction);

            void drawBBox(GeometryContainer&) const;
            void drawBaseBones(GeometryContainer&) const;

        private:
            ModelInstanceDisplayer* findModelInstanceDisplayer(const Model&) const;
            void clearDisplayers();
            void removeModelFromDisplayer(Model&, ModelInstanceDisplayer&);
            void addModelToDisplayer(Model&, ModelInstanceDisplayer&);

            void observeModelUpdate(Model&);
            void unobserveModelUpdate(Model&);

            bool isInitialized;

            std::string vertexShaderName;
            std::string geometryShaderName;
            std::string fragmentShaderName;
            std::unique_ptr<ShaderConstants> shaderConstants;

            DisplayMode displayMode;
            std::unique_ptr<Shader> modelShader;

            std::unique_ptr<CustomModelShaderVariable> customShaderVariable;
            bool depthTestEnabled;
            bool depthWriteEnabled;
            bool enableFaceCull;
            bool displayModelsInOrder;
            std::vector<BlendFunction> blendFunctions;
            std::unique_ptr<MeshFilter> meshFilter;

            RenderTarget* renderTarget;
            std::vector<Model*> models;
            std::unordered_set<ModelInstanceDisplayer*> activeModelDisplayers;
            std::unordered_map<Model*, std::unique_ptr<ModelInstanceDisplayer>> modelDisplayers;
            std::unordered_map<std::size_t, std::unique_ptr<ModelInstanceDisplayer>> modelInstanceDisplayers;
    };

}
