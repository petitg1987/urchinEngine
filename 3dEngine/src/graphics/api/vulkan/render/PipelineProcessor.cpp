#include <graphics/api/vulkan/render/PipelineProcessor.h>
#include <graphics/api/vulkan/render/shader/Shader.h>

namespace urchin {

    PipelineProcessor::PipelineProcessor(std::string name, RenderTarget& renderTarget, const Shader& shader) :
            name(std::move(name)),
            renderTarget(renderTarget),
            shader(shader),
            bIsEnabled(true),
            renderingOrder(0) {

    }

    PipelineProcessor::~PipelineProcessor() {
        renderTarget.removeProcessor(this);
    }

    const std::string& PipelineProcessor::getName() const {
        return name;
    }

    const RenderTarget& PipelineProcessor::getRenderTarget() const {
        return renderTarget;
    }

    const Shader& PipelineProcessor::getShader() const {
        return shader;
    }

    bool PipelineProcessor::isEnabled() const {
        return bIsEnabled;
    }

    void PipelineProcessor::enableRenderer(unsigned int renderingOrder) {
        this->bIsEnabled = true;
        this->renderingOrder = renderingOrder;
        renderTarget.notifyProcessorEnabled(this);
    }

    void PipelineProcessor::disableRenderer() {
        bIsEnabled = false;
        renderTarget.notifyProcessorDisabled(this);
    }

    unsigned int PipelineProcessor::getRenderingOrder() const {
        return renderingOrder;
    }
}