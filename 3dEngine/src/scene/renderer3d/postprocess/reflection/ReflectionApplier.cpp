#include <scene/renderer3d/postprocess/reflection/ReflectionApplier.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>
#include <graphics/render/target/NullRenderTarget.h>

namespace urchin {

    ReflectionApplier::ReflectionApplier(bool useNullRenderTarget) :
            useNullRenderTarget(useNullRenderTarget) {

    }

    ReflectionApplier::~ReflectionApplier() {
        clearRenderingObjects();
    }

    void ReflectionApplier::onCameraProjectionUpdate(const Camera& camera) {
        projection.projectionMatrix = camera.getProjectionMatrix();
        projection.inverseProjectionMatrix = camera.getProjectionInverseMatrix();

        createOrUpdateRenderingObjects();
    }

    void ReflectionApplier::refreshInputTexture(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& normalAndAmbientTexture,
                                                const std::shared_ptr<Texture>& materialTexture, const std::shared_ptr<Texture>& illuminatedTexture) {
        if (depthTexture.get() != this->depthTexture.get()
                || normalAndAmbientTexture.get() != this->normalAndAmbientTexture.get()
                || materialTexture.get() != this->materialTexture.get()
                || illuminatedTexture.get() != this->illuminatedTexture.get()) {
            this->depthTexture = depthTexture;
            this->normalAndAmbientTexture = normalAndAmbientTexture;
            this->materialTexture = materialTexture;
            this->illuminatedTexture = illuminatedTexture;

            createOrUpdateRenderingObjects();
        }
    }

    void ReflectionApplier::createOrUpdateRenderingObjects() {
        clearRenderingObjects();

        if (depthTexture) {
            createOrUpdateRenderTargets();
            createOrUpdateRenderers();
        }
    }

    void ReflectionApplier::clearRenderingObjects() {
        reflectionColorRenderer.reset();
        if (reflectionColorRenderTarget) {
            reflectionColorRenderTarget->cleanup();
            reflectionColorRenderTarget.reset();
        }

        reflectionCombineRenderer.reset();
        if (reflectionCombineRenderTarget) {
            reflectionCombineRenderTarget->cleanup();
            reflectionCombineRenderTarget.reset();
        }
    }

    void ReflectionApplier::createOrUpdateRenderTargets() {
        reflectionColorOutputTexture = Texture::build("reflectionColor", depthTexture->getWidth(), depthTexture->getHeight(), TextureFormat::RGBA_16_FLOAT);
        if (useNullRenderTarget) {
            reflectionColorRenderTarget = std::make_unique<NullRenderTarget>(depthTexture->getWidth(), depthTexture->getHeight());
        } else {
            reflectionColorRenderTarget = std::make_unique<OffscreenRender>("reflectionColor", RenderTarget::NO_DEPTH_ATTACHMENT);
            static_cast<OffscreenRender*>(reflectionColorRenderTarget.get())->addOutputTexture(reflectionColorOutputTexture);
            reflectionColorRenderTarget->initialize();
        }

        reflectionCombineOutputTexture = Texture::build("reflectionCombine", depthTexture->getWidth(), depthTexture->getHeight(), TextureFormat::RGBA_16_FLOAT);
        if (useNullRenderTarget) {
            reflectionCombineRenderTarget = std::make_unique<NullRenderTarget>(depthTexture->getWidth(), depthTexture->getHeight());
        } else {
            reflectionCombineRenderTarget = std::make_unique<OffscreenRender>("reflectionCombine", RenderTarget::NO_DEPTH_ATTACHMENT);
            static_cast<OffscreenRender*>(reflectionCombineRenderTarget.get())->addOutputTexture(reflectionCombineOutputTexture);
            reflectionCombineRenderTarget->initialize();
        }
    }

    void ReflectionApplier::createOrUpdateRenderers() {
        createOrUpdateShaders();

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };

        reflectionColorRenderer = GenericRendererBuilder::create("reflectionColor", *reflectionColorRenderTarget, *reflectionColorShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(PROJECTION_UNIFORM_BINDING, sizeof(projection), &projection)
                ->addUniformTextureReader(DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(depthTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(NORMAL_AMBIENT_TEX_UNIFORM_BINDING, TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(MATERIAL_TEX_UNIFORM_BINDING, TextureReader::build(materialTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(R_COLOR_ILLUMINATED_TEX_UNIFORM_BINDING, TextureReader::build(illuminatedTexture, TextureParam::buildLinear()))
                ->build();

        reflectionCombineRenderer = GenericRendererBuilder::create("reflectionCombine", *reflectionCombineRenderTarget, *reflectionCombineShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(REFLECTION_COLOR_TEX_UNIFORM_BINDING, TextureReader::build(reflectionColorOutputTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(R_COMBINE_ILLUMINATED_TEX_UNIFORM_BINDING, TextureReader::build(illuminatedTexture, TextureParam::buildLinear()))
                ->build();
    }

    void ReflectionApplier::createOrUpdateShaders() {
        if (reflectionColorRenderTarget->isValidRenderTarget()) {
            reflectionColorShader = ShaderBuilder::createShader("reflectionColor.vert.spv", "reflectionColor.frag.spv");
        } else {
            reflectionColorShader = ShaderBuilder::createNullShader();
        }

        if (reflectionCombineRenderTarget->isValidRenderTarget()) {
            reflectionCombineShader = ShaderBuilder::createShader("reflectionCombine.vert.spv", "reflectionCombine.frag.spv");
        } else {
            reflectionCombineShader = ShaderBuilder::createNullShader();
        }
    }

    const std::shared_ptr<Texture>& ReflectionApplier::getOutputTexture() const {
        return reflectionCombineOutputTexture;
    }

    void ReflectionApplier::applyReflection(std::uint32_t frameIndex, unsigned int numDependenciesToReflectionTexture) const {
        ScopeProfiler sp(Profiler::graphic(), "applySSR");

        unsigned int numDependenciesToReflectionColorTexture = 1;
        reflectionColorRenderTarget->render(frameIndex, numDependenciesToReflectionColorTexture);
        reflectionCombineRenderTarget->render(frameIndex, numDependenciesToReflectionTexture);
    }

}
