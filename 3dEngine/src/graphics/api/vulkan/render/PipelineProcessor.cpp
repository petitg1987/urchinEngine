#include <graphics/api/vulkan/render/PipelineProcessor.h>
#include <graphics/api/vulkan/render/shader/Shader.h>

namespace urchin {

    PipelineProcessor::PipelineProcessor(std::string name, RenderTarget& renderTarget, const Shader& shader) :
            mName(std::move(name)),
            mRenderTarget(renderTarget),
            mShader(shader),
            bIsEnabled(true),
            renderingOrder(0) {

    }

    const std::string& PipelineProcessor::getName() const {
        return mName;
    }

    const RenderTarget& PipelineProcessor::getRenderTarget() const {
        return mRenderTarget;
    }

    RenderTarget& PipelineProcessor::renderTarget() {
        return mRenderTarget;
    }

    const Shader& PipelineProcessor::getShader() const {
        return mShader;
    }

    bool PipelineProcessor::isEnabled() const {
        return bIsEnabled;
    }

    void PipelineProcessor::enableRenderer(unsigned int renderingOrder) {
        this->bIsEnabled = true;
        this->renderingOrder = renderingOrder;
        mRenderTarget.notifyProcessorEnabled(this);
    }

    void PipelineProcessor::disableRenderer() {
        bIsEnabled = false;
        mRenderTarget.notifyProcessorDisabled(this);
    }

    unsigned int PipelineProcessor::getRenderingOrder() const {
        return renderingOrder;
    }
}