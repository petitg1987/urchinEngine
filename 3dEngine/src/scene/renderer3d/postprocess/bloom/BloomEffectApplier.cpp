#include <scene/renderer3d/postprocess/bloom/BloomEffectApplier.h>
#include <graphic/render/target/OffscreenRender.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>

namespace urchin {

    BloomEffectApplier::BloomEffectApplier() :
            sceneWidth(0),
            sceneHeight(0),
            finalRenderTarget(nullptr) {

    }

    BloomEffectApplier::~BloomEffectApplier() {
        clearRendering();
    }

    void BloomEffectApplier::onTextureUpdate(const std::shared_ptr<Texture>& lightingPassTexture, std::optional<RenderTarget*> customRenderTarget) {
        this->lightingPassTexture = lightingPassTexture;
        this->sceneWidth = lightingPassTexture->getWidth();
        this->sceneHeight = lightingPassTexture->getHeight();

        clearRendering();
        if (customRenderTarget.has_value()) {
            this->finalRenderTarget = customRenderTarget.value();
        } else {
            bloomedTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_8_INT, nullptr);
            offscreenBloomRenderTarget = std::make_unique<OffscreenRender>("bloom - combine", RenderTarget::NO_DEPTH_ATTACHMENT);
            offscreenBloomRenderTarget->resetTextures();
            offscreenBloomRenderTarget->addTexture(bloomedTexture);
            offscreenBloomRenderTarget->initialize();

            this->finalRenderTarget = offscreenBloomRenderTarget.get();
        }

        refreshRenderer();
    }

    void BloomEffectApplier::refreshRenderer() {
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        bloomPreFilterShader = ShaderBuilder::createShader("bloomPreFilter.vert.spv", "", "bloomPreFilter.frag.spv");
        preFilterRenderer = GenericRendererBuilder::create("bloom - prefilter ", *finalRenderTarget, *bloomPreFilterShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(lightingPassTexture, TextureParam::buildNearest())) //binding 0
                ->build();

        //TODO combine: ACES tone mapping ?
    }

    void BloomEffectApplier::clearRendering() {
        preFilterRenderer.reset();
        if(offscreenBloomRenderTarget) {
            offscreenBloomRenderTarget->cleanup();
            offscreenBloomRenderTarget.reset();
        }
        bloomedTexture.reset();
    }

    const std::shared_ptr<Texture>& BloomEffectApplier::getBloomedTexture() const {
        if (!bloomedTexture) {
            throw std::runtime_error("No bloomed texture available because rendering has been done in a custom render target");
        }
        return bloomedTexture;
    }

    void BloomEffectApplier::applyBloom() {
        if (bloomedTexture) {
            finalRenderTarget->render();
        } else {
            preFilterRenderer->enableRenderer();
        }
    }

}