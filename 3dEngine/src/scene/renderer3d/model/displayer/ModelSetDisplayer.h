#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <UrchinCommon.h>

#include "graphics/api/GraphicsApi.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/displayer/DisplayMode.h"
#include "scene/renderer3d/model/displayer/ModelInstanceDisplayer.h"
#include "scene/renderer3d/model/displayer/CustomModelShaderVariable.h"

namespace urchin {

    /**
    * Allow to display models given in parameter depending on a configuration (view matrix, projection matrix, display mode...)
    */
    class ModelSetDisplayer final : public Observer {
        public:
            using ModelSortFunction = bool (*)(const Model*, const Model*, const void*);

            explicit ModelSetDisplayer(DisplayMode displayMode);
            ~ModelSetDisplayer() override;

            void initialize(RenderTarget&);
            void setupShader(std::string, std::string, std::unique_ptr<ShaderConstants>);
            void setupCustomShaderVariable(std::unique_ptr<CustomModelShaderVariable>);
            CustomModelShaderVariable* getCustomShaderVariable() const;
            void setupDepthOperations(bool, bool);
            void setupFaceCull(bool);
            void setupBlendFunctions(const std::vector<BlendFunction>&);
            void setupLayerIndexDataInShader(bool);
            void setupMeshFilter(std::unique_ptr<MeshFilter>);

            void notify(Observable*, int) override;

            bool registerModel(Model*);
            void unregisterModel(Model*);
            void addModelToDisplay(Model*, std::bitset<8> = std::bitset<8>(ULLONG_MAX));
            void setModelsToDisplay(std::span<Model* const>);
            void resetModelsToDisplay();
            std::span<Model* const> getModelsToDisplay() const;
            bool isDisplayerExist(const Model&) const;

            void prepareRendering(unsigned int, const Matrix4<float>&);
            void prepareRendering(unsigned int&, const Matrix4<float>&, const ModelSortFunction&, const void*);

            void drawBBox(GeometryContainer&) const;
            void drawBaseBones(GeometryContainer&) const;

        private:
            ModelInstanceDisplayer* findModelInstanceDisplayer(const Model&) const;
            ModelInstanceDisplayer* createOrUseDisplayerForModel(Model*);
            void detachModelFromDisplayer(Model*, ModelInstanceDisplayer*);

            void observeModelUpdate(Model&);
            void unobserveModelUpdate(Model&);

            void purgeUnusedInstanceDisplayers();

            bool isInitialized;

            std::string vertexShaderName;
            std::string fragmentShaderName;
            std::unique_ptr<ShaderConstants> shaderConstants;

            DisplayMode displayMode;
            std::unique_ptr<Shader> modelShader;

            std::unique_ptr<CustomModelShaderVariable> customShaderVariable;
            bool depthTestEnabled;
            bool depthWriteEnabled;
            bool enableFaceCull;
            std::vector<BlendFunction> blendFunctions;
            bool enableLayerIndexDataInShader;

            std::unique_ptr<MeshFilter> meshFilter;

            RenderTarget* renderTarget;
            std::unordered_set<Model*> registeredModels;
            std::vector<Model*> modelsToDisplay;
            EverGrowHashSet<ModelInstanceDisplayer*> activeInstanceDisplayers;
            std::unordered_map<Model*, std::unique_ptr<ModelInstanceDisplayer>> exclusiveInstanceDisplayers;
            std::unordered_map<std::size_t, std::unique_ptr<ModelInstanceDisplayer>> shareableInstanceDisplayers;
    };

}
