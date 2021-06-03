#include <UrchinCommon.h>

#include <graphic/setup/GraphicService.h>
#define VMA_IMPLEMENTATION
#include <libs/vma/vk_mem_alloc.h>
#include <graphic/setup/handler/QueueHandler.h>
using namespace urchin;

namespace urchin {

    //static
    bool GraphicService::useUniqueSurface = false;
    VkInstance GraphicService::vkInstance = nullptr;
    VkSurfaceKHR GraphicService::surface = nullptr;

    GraphicService::GraphicService() :
            apiGraphicInitialized(false),
            vulkanVersion(VK_API_VERSION_1_1), //must be aligned with glslc target environment
            allocateCommandPool(nullptr),
            allocator(nullptr) {

    }

    GraphicService::~GraphicService() {
        vmaDestroyAllocator(allocator);
        vkDestroyCommandPool(getDevices().getLogicalDevice(), allocateCommandPool, nullptr);
        deviceHandler.cleanup();
        validationLayer.cleanup();
        if (!useUniqueSurface) {
            destroySurface();
        }
    }

    /**
     * Some Windows manager libraries do not support well the recreation of a Vulkan surface.
     * Therefore, this feature allows to reuse the same surface (and his associated instance) even after the destruction of this service.
     * When this feature is enable, the 'destroySurface' method must be explicitly called.
     */
    void GraphicService::enableUniqueSurface() {
        useUniqueSurface = true;
    }

    void GraphicService::destroySurface() {
        if (surface) {
            vkDestroySurfaceKHR(vkInstance, surface, nullptr);
            surface = nullptr;
        }

        if (vkInstance) {
            vkDestroyInstance(vkInstance, nullptr);
            vkInstance = nullptr;
        }
    }

    void GraphicService::initialize(const std::vector<std::string>& windowRequiredExtensions, const std::unique_ptr<SurfaceCreator>& surfaceCreator, std::unique_ptr<FramebufferSizeRetriever> framebufferSizeRetriever) {
        this->framebufferSizeRetriever = std::move(framebufferSizeRetriever);

        createInstance(windowRequiredExtensions);
        validationLayer.initializeDebugMessenger(vkInstance);
        if (!surface) {
            surface = surfaceCreator->createSurface(vkInstance);
        }
        deviceHandler.initializeDevices(vkInstance, surface);
        queueHandler.initializeQueueFamilies(deviceHandler.getPhysicalDevice(), surface);
        queueHandler.initializeQueues(deviceHandler.getLogicalDevice());
        createAllocateCommandPool();
        createAllocator();

        apiGraphicInitialized = true;
    }

    void GraphicService::createInstance(const std::vector<std::string>& windowRequiredExtensions) {
        if (vkInstance) {
            return;
        }

        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "VulkanApp-ff268de9-d92a-47b4-b6b0-8df93853565f";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Engine-04067ff4-d202-4e33-8ca0-c4f5e929affd";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = vulkanVersion;

        std::vector<std::string> requiredExtensionsString = VectorUtil::merge(windowRequiredExtensions, validationLayer.getRequiredExtensions());
        std::vector<const char*> requiredExtensions;
        requiredExtensions.reserve(requiredExtensionsString.size());
        for (std::string const& requiredExtensionString : requiredExtensionsString) {
            requiredExtensions.emplace_back(requiredExtensionString.data());
        }

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

    const std::unique_ptr<FramebufferSizeRetriever>& GraphicService::getFramebufferSizeRetriever() const {
        return framebufferSizeRetriever;
    }

    VkSurfaceKHR GraphicService::getSurface() {
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
