#include <graphics/api/vulkan/GraphicsApiService.h>
#define VMA_IMPLEMENTATION
#include <libs/vma/vk_mem_alloc.h>
#include <graphics/api/vulkan/helper/MemoryHelper.h>
#include <graphics/api/vulkan/render/pipeline/PipelineContainer.h>
#include <graphics/api/vulkan/setup/GraphicsSetupService.h>

namespace urchin {

    GraphicsApiService::GraphicsApiService(const std::vector<std::string>& windowRequiredExtensions, std::unique_ptr<SurfaceCreator> surfaceCreator, FramebufferSizeRetriever& framebufferSizeRetriever) {
        GraphicsSetupService::instance().initialize(windowRequiredExtensions, std::move(surfaceCreator), framebufferSizeRetriever);
    }

    GraphicsApiService::~GraphicsApiService() {
        PipelineContainer::instance().cleanPipelines();
        GraphicsSetupService::instance().cleanup();
    }

    void GraphicsApiService::frameStart(uint32_t frameCount) const {
        //required for memory budget: https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/staying_within_budget.html
        vmaSetCurrentFrameIndex(GraphicsSetupService::instance().getAllocator(), frameCount);
    }

    void GraphicsApiService::frameEnd() const {
        MemoryHelper::checkMemoryUsage();
        PipelineContainer::instance().cleanPipelines();
    }

}
