#include <memory>

#include <resources/material/UvScale.h>
#include <scene/renderer3d/model/displayer/ModelDisplayer.h>
#include <api/render/GenericRendererBuilder.h>

namespace urchin {

    ModelDisplayer::ModelDisplayer(Model* model, DisplayMode displayMode, RenderTarget& renderTarget, const Shader& shader) :
            isInitialized(false),
            model(model),
            displayMode(displayMode),
            renderTarget(renderTarget),
            shader(shader),
            positioningData({}),
            materialData({}),
            customShaderVariable(nullptr),
            depthTestEnabled(true),
            depthWriteEnabled(true),
            enableFaceCull(true) {

    }

    ModelDisplayer::~ModelDisplayer() {
        model->removeObserver(this, Model::MATERIAL_UPDATED);
        model->removeObserver(this, Model::MESH_UPDATED);
    }

    void ModelDisplayer::setupCustomShaderVariable(CustomModelShaderVariable* customShaderVariable) {
        if (customShaderVariable && isInitialized) {
            throw std::runtime_error("Can not define a custom model shader variable on an initialized model displayer: " + model->getConstMeshes()->getName());
        }
        this->customShaderVariable = customShaderVariable;
    }

    void ModelDisplayer::setupDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        if (isInitialized) {
            throw std::runtime_error("Can not define depth operations on an initialized model displayer: " + model->getConstMeshes()->getName());
        }
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
    }

    void ModelDisplayer::setupFaceCull(bool enableFaceCull) {
        if (isInitialized) {
            throw std::runtime_error("Can not define face cull flag on an initialized model displayer: " + model->getConstMeshes()->getName());
        }
        this->enableFaceCull = enableFaceCull;
    }

    void ModelDisplayer::setupBlendFunctions(const std::vector<BlendFunction>& blendFunctions) {
        if (!blendFunctions.empty() && isInitialized) {
            throw std::runtime_error("Can not define blend functions on an initialized model displayer: " + model->getConstMeshes()->getName());
        }
        this->blendFunctions = blendFunctions;
    }

    void ModelDisplayer::initialize() {
        if (isInitialized) {
            throw std::runtime_error("Model displayer is already initialized: " + model->getConstMeshes()->getName());
        }

        for (unsigned int i = 0; i < model->getMeshes()->getNumberMeshes(); ++i) {
            const ConstMesh& constMesh = model->getConstMeshes()->getConstMesh(i);
            const Mesh& mesh = model->getMeshes()->getMesh(i);
            auto meshName = model->getMeshes()->getConstMeshes().getMeshesName();

            fillMaterialData(mesh);

            auto meshRendererBuilder = GenericRendererBuilder::create("mesh - " + meshName, renderTarget, this->shader, ShapeType::TRIANGLE)
                    ->addData(mesh.getVertices())
                    ->indices(constMesh.getTrianglesIndices())
                    ->addUniformData(sizeof(positioningData), &positioningData) //binding 0
                    ->addUniformData(sizeof(materialData), &materialData); //binding 1 (only used in DEFAULT_MODE)

                if (customShaderVariable) {
                    customShaderVariable->setupMeshRenderer(meshRendererBuilder); //binding 2 & 3 (optional)
                }
                int missingUniformData = 4 - (int)meshRendererBuilder->getUniformData().size();
                assert(missingUniformData >= 0);
                for (int i = 0; i < missingUniformData; ++i) {
                    int customDummyValue = 0;
                    meshRendererBuilder->addUniformData(sizeof(customDummyValue), &customDummyValue); //binding 2 & 3
                }

                if (depthTestEnabled) {
                    meshRendererBuilder->enableDepthTest();
                }
                if (depthWriteEnabled) {
                    meshRendererBuilder->enableDepthWrite();
                }
                if (!enableFaceCull) {
                    meshRendererBuilder->disableCullFace();
                }
                if (!blendFunctions.empty()) {
                    meshRendererBuilder->enableTransparency(blendFunctions);
                }

                if (displayMode == DEFAULT_MODE) {
                    const UvScale& uvScale = mesh.getMaterial().getUvScale();
                    meshRendererBuilder
                            ->addData(uvScale.hasScaling() ? scaleUv(constMesh.getUvTexture(), uvScale) : constMesh.getUvTexture())
                            ->addData(mesh.getNormals())
                            ->addData(mesh.getTangents())
                            ->addUniformTextureReader(TextureReader::build(mesh.getMaterial().getDiffuseTexture(), buildTextureParam(mesh))) //binding 4
                            ->addUniformTextureReader(TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh))); //binding 5
                }

                meshRenderers.push_back(meshRendererBuilder->build());
        }

        model->addObserver(this, Model::MESH_UPDATED);
        model->addObserver(this, Model::MATERIAL_UPDATED);
        model->addObserver(this, Model::SCALE_UPDATED);

        isInitialized = true;
    }

    void ModelDisplayer::fillMaterialData(const Mesh& mesh) {
        materialData.encodedEmissiveFactor = MathFunction::clamp(mesh.getMaterial().getEmissiveFactor() / Material::MAX_EMISSIVE_FACTOR, 0.0f, 1.0f);
        materialData.ambientFactor = mesh.getMaterial().getAmbientFactor();
    }

    std::vector<Point2<float>> ModelDisplayer::scaleUv(const std::vector<Point2<float>>& uvTexture, const UvScale& uvScale) const { //TODO update material could affect this method
        std::vector<Point2<float>> scaledUvTexture;
        scaledUvTexture.reserve(uvTexture.size());

        const Vector3<float> scale = model->getTransform().getScale();
        for(const Point2<float>& uv : uvTexture) {
            scaledUvTexture.emplace_back(uvScale.scaleU(uv.X, scale), uvScale.scaleV(uv.Y, scale));
        }
        return scaledUvTexture;
    }

    TextureParam ModelDisplayer::buildTextureParam(const Mesh& mesh) const {
        auto textureReadMode = mesh.getMaterial().isRepeatTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
        return TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
    }

    void ModelDisplayer::notify(Observable* observable, int notificationType) {
        if (const auto* model = dynamic_cast<Model*>(observable)) {
            if (notificationType == Model::MESH_UPDATED) {
                unsigned int meshIndex = 0;
                for (const auto& meshRenderer : meshRenderers) {
                    const Mesh& mesh = model->getMeshes()->getMesh(meshIndex);
                    meshRenderer->updateData(0, mesh.getVertices());
                    if (displayMode == DEFAULT_MODE) {
                        meshRenderer->updateData(2, mesh.getNormals());
                        meshRenderer->updateData(3, mesh.getTangents());
                    }

                    meshIndex++;
                }
            } else if (notificationType == Model::MATERIAL_UPDATED) {
                if (displayMode == DEFAULT_MODE) {
                    unsigned int meshIndex = 0;
                    for (const auto& meshRenderer : meshRenderers) {
                        const Mesh& mesh = model->getMeshes()->getMesh(meshIndex);

                        fillMaterialData(mesh);
                        meshRenderer->updateUniformData(1, &materialData);

                        if (meshRenderer->getUniformTextureReader(0)->getTexture() != mesh.getMaterial().getDiffuseTexture().get()) {
                            meshRenderer->updateUniformTextureReader(0, TextureReader::build(mesh.getMaterial().getDiffuseTexture(), buildTextureParam(mesh)));
                        }
                        if (meshRenderer->getUniformTextureReader(1)->getTexture() != mesh.getMaterial().getNormalTexture().get()) {
                            meshRenderer->updateUniformTextureReader(1, TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh)));
                        }

                        meshIndex++;
                    }
                }
            } else if (notificationType == Model::SCALE_UPDATED) {
                if (displayMode == DEFAULT_MODE) {
                    unsigned int meshIndex = 0;
                    for (const auto& meshRenderer: meshRenderers) {
                        const ConstMesh& constMesh = model->getConstMeshes()->getConstMesh(meshIndex);
                        const Mesh& mesh = model->getMeshes()->getMesh(meshIndex);
                        const UvScale& uvScale = mesh.getMaterial().getUvScale();
                        meshRenderer->updateData(1, uvScale.hasScaling() ? scaleUv(constMesh.getUvTexture(), uvScale) : constMesh.getUvTexture());

                        meshIndex++;
                    }
                }
            }
        }
    }

    void ModelDisplayer::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix, const MeshFilter* meshFilter) const {
        unsigned int meshIndex = 0;
        for (auto& meshRenderer : meshRenderers) {
            const Mesh& mesh = model->getMeshes()->getMesh(meshIndex++);
            if (meshFilter && !meshFilter->isAccepted(mesh)) {
                continue;
            }

            positioningData.projectionViewMatrix = projectionViewMatrix;
            positioningData.modelMatrix = model->getTransform().getTransformMatrix();
            positioningData.normalMatrix = model->getTransform().getTransformMatrix().inverse().transpose();
            meshRenderer->updateUniformData(0, &positioningData);

            if (customShaderVariable) {
                customShaderVariable->loadCustomShaderVariables(*meshRenderer);
            }

            meshRenderer->enableRenderer(renderingOrder);
        }
    }

    void ModelDisplayer::drawBBox(GeometryContainer& geometryContainer) {
        if (aabboxModel) {
            geometryContainer.removeGeometry(*aabboxModel);
        }

        aabboxModel = std::make_shared<AABBoxModel>(model->getAABBox());
        geometryContainer.addGeometry(aabboxModel);
    }

    void ModelDisplayer::drawBaseBones(GeometryContainer& geometryContainer, const MeshFilter* meshFilter) const {
        if (model->getMeshes()) {
            for (unsigned int m = 0; m < model->getMeshes()->getNumberMeshes(); ++m) {
                const Mesh& mesh = model->getMeshes()->getMesh(m);
                if (!meshFilter || meshFilter->isAccepted(mesh)) {
                    model->getMeshes()->getMesh(m).drawBaseBones(geometryContainer, model->getTransform().getTransformMatrix());
                }
            }
        }
    }

}
