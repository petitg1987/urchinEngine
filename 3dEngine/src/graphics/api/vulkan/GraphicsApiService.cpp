#include <graphics/api/vulkan/GraphicsApiService.h>
#define VMA_IMPLEMENTATION
#include <libs/vma/vk_mem_alloc.h>
#include <graphics/api/vulkan/helper/MemoryHelper.h>
#include <graphics/api/vulkan/render/pipeline/PipelineContainer.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>

namespace urchin {

    GraphicsApiService::GraphicsApiService() {
        GraphicsSetupService::instance();
    }

    void GraphicsApiService::enableUniqueSurface() {
        GraphicsSetupService::enableUniqueSurface();
    }

    void GraphicsApiService::destroySurface() {
        GraphicsSetupService::destroySurface();
    }

    void GraphicsApiService::initialize(const std::vector<std::string>& windowRequiredExtensions, std::unique_ptr<SurfaceCreator> surfaceCreator, FramebufferSizeRetriever& framebufferSizeRetriever) {
        GraphicsSetupService::instance().initialize(windowRequiredExtensions, std::move(surfaceCreator), framebufferSizeRetriever);
    }

    void GraphicsApiService::frameStart(std::uint32_t frameIndex) const {
        //required for memory budget: https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/staying_within_budget.html
        vmaSetCurrentFrameIndex(GraphicsSetupService::instance().getAllocator(), frameIndex);
    }

    void GraphicsApiService::frameEnd() const {
        MemoryHelper::checkMemoryUsage();
        PipelineContainer::instance().cleanPipelines();
    }

}
