#include <scene/renderer3d/postprocess/reflection/ReflectionApplier.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>
#include <graphics/render/target/NullRenderTarget.h>

namespace urchin {

    ReflectionApplier::ReflectionApplier(bool useNullRenderTarget) :
            useNullRenderTarget(useNullRenderTarget) {

    }

    void ReflectionApplier::refreshInputTexture(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& normalAndAmbientTexture,
                                                const std::shared_ptr<Texture>& materialTexture, const std::shared_ptr<Texture>& illuminatedTexture) {
        if (depthTexture.get() != this->depthTexture.get()
                || normalAndAmbientTexture.get() != this->normalAndAmbientTexture.get()
                || materialTexture.get() != this->materialTexture.get()
                || illuminatedTexture.get() != this->illuminatedTexture.get()) {
            this->depthTexture = depthTexture;
            this->normalAndAmbientTexture = normalAndAmbientTexture; //TODO review doc on dependencies graph
            this->materialTexture = materialTexture; //TODO review doc on dependencies graph
            this->illuminatedTexture = illuminatedTexture;

            clearRenderer();
            outputTexture = Texture::build("reflection", depthTexture->getWidth(), depthTexture->getHeight(), TextureFormat::RGBA_16_FLOAT);
            if (useNullRenderTarget) {
                renderTarget = std::make_unique<NullRenderTarget>(depthTexture->getWidth(), depthTexture->getHeight());
            } else {
                renderTarget = std::make_unique<OffscreenRender>("reflection", RenderTarget::NO_DEPTH_ATTACHMENT);
                static_cast<OffscreenRender*>(renderTarget.get())->addOutputTexture(outputTexture);
                renderTarget->initialize();
            }

            createOrUpdateRenderer();
        }
    }

    void ReflectionApplier::clearRenderer() {
        renderer.reset();

        if (renderTarget) {
            renderTarget->cleanup();
            renderTarget.reset();
        }
    }

    void ReflectionApplier::createOrUpdateRenderer() {
        createOrUpdateShader();

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        renderer = GenericRendererBuilder::create("reflection", *renderTarget, *shader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(INPUT_TEX_UNIFORM_BINDING, TextureReader::build(illuminatedTexture, TextureParam::buildLinear()))
                ->build();
    }

    void ReflectionApplier::createOrUpdateShader() {
        if (renderTarget->isValidRenderTarget()) {
            shader = ShaderBuilder::createShader("reflection.vert.spv", "reflection.frag.spv");
        } else {
            shader = ShaderBuilder::createNullShader();
        }
    }

    const std::shared_ptr<Texture>& ReflectionApplier::getOutputTexture() const {
        return outputTexture;
    }

    void ReflectionApplier::applyReflection(std::uint32_t frameIndex, unsigned int numDependenciesToReflectionTexture) const {
        ScopeProfiler sp(Profiler::graphic(), "applySSR");

        renderTarget->render(frameIndex, numDependenciesToReflectionTexture);
    }

}
