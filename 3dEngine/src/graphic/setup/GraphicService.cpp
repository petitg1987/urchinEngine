#include "UrchinCommon.h"

#include "GraphicService.h"
#define VMA_IMPLEMENTATION
#include "libs/vma/vk_mem_alloc.h"
#include "graphic/setup/handler/QueueHandler.h"
using namespace urchin;

namespace urchin {

    GraphicService::GraphicService() :
            apiGraphicInitialized(false),
            vulkanVersion(VK_API_VERSION_1_1),
            vkInstance(nullptr),
            surface(nullptr),
            allocateCommandPool(nullptr),
            allocator(nullptr) {

    }

    GraphicService::~GraphicService() {
        vmaDestroyAllocator(allocator);
        vkDestroyCommandPool(getDevices().getLogicalDevice(), allocateCommandPool, nullptr);
        deviceHandler.cleanup();
        validationLayer.cleanup();
        vkDestroySurfaceKHR(vkInstance, surface, nullptr);
        vkDestroyInstance(vkInstance, nullptr);
    }

    void GraphicService::initialize(const std::vector<const char*>& windowRequiredExtensions,
                                    const std::unique_ptr<SurfaceCreator>& surfaceCreator,
                                    std::unique_ptr<FramebufferSizeRetriever> framebufferSizeRetriever) {
        this->framebufferSizeRetriever = std::move(framebufferSizeRetriever);

        createInstance(windowRequiredExtensions);
        validationLayer.initializeDebugMessenger(vkInstance);
        surface = surfaceCreator->createSurface(vkInstance);
        deviceHandler.initializeDevices(vkInstance, surface);
        queueHandler.initializeQueueFamilies(deviceHandler.getPhysicalDevice(), surface);
        queueHandler.initializeQueues(deviceHandler.getLogicalDevice());
        createAllocateCommandPool();
        createAllocator();

        apiGraphicInitialized = true;
    }

    const std::unique_ptr<FramebufferSizeRetriever>& GraphicService::getFramebufferSizeRetriever() const {
        return framebufferSizeRetriever;
    }

    VkSurfaceKHR GraphicService::getSurface() const {
        assert(surface);
        return surface;
    }

    DeviceHandler GraphicService::getDevices() const {
        assert(apiGraphicInitialized);
        return deviceHandler;
    }

    QueueHandler GraphicService::getQueues() const {
        assert(apiGraphicInitialized);
        return queueHandler;
    }

    VkCommandPool GraphicService::getAllocateCommandPool() const {
        assert(apiGraphicInitialized);
        return allocateCommandPool;
    }

    VmaAllocator GraphicService::getAllocator() const {
        assert(allocator);
        return allocator;
    }

    const ValidationLayer& GraphicService::getValidationLayer() const {
        return validationLayer;
    }

    void GraphicService::createInstance(const std::vector<const char*>& windowRequiredExtensions) {
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "VulkanApp-ff268de9-d92a-47b4-b6b0-8df93853565f";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Engine-04067ff4-d202-4e33-8ca0-c4f5e929affd";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = vulkanVersion;

        std::vector<const char*> requiredExtensions = VectorUtil::merge(windowRequiredExtensions, validationLayer.getRequiredExtensions());
        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
        validationLayer.initializeDebugMessengerForInstance(instanceCreateInfo);

        VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance);
        if(result != VK_SUCCESS) {
            throw UserAuthorityException("Failed to create Vulkan instance with error code: " + std::to_string(result), "Upgrade your graphic drivers to support better Vulkan");
        }
    }

    void GraphicService::createAllocateCommandPool() {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueHandler.getGraphicsQueueFamily();
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        VkResult result = vkCreateCommandPool(deviceHandler.getLogicalDevice(), &poolInfo, nullptr, &allocateCommandPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool with error code: " + std::to_string(result));
        }
    }

    void GraphicService::createAllocator() {
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.vulkanApiVersion = vulkanVersion;
        allocatorInfo.physicalDevice = deviceHandler.getPhysicalDevice();
        allocatorInfo.device = deviceHandler.getLogicalDevice();
        allocatorInfo.instance = vkInstance;

        vmaCreateAllocator(&allocatorInfo, &allocator);
    }

}