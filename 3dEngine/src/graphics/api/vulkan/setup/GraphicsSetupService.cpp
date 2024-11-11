#include <libs/vkenum/vk_enum.h>
#include <UrchinCommon.h>

#include <graphics/api/vulkan/setup/GraphicsSetupService.h>
#include <graphics/api/vulkan/setup/handler/QueueHandler.h>
#include <libs/vma/vk_mem_alloc.h>
using namespace urchin;

namespace urchin {

    //static
    VkInstance GraphicsSetupService::vkInstance = nullptr;
    VkSurfaceKHR GraphicsSetupService::surface = nullptr;

    GraphicsSetupService& GraphicsSetupService::instance() {
        static GraphicsSetupService instance;
        return instance;
    }

    GraphicsSetupService::GraphicsSetupService() :
            framebufferSizeRetriever(nullptr),
            apiGraphicInitialized(false),
            vulkanVersion(VK_API_VERSION_1_3), //must be aligned with glslc target environment
            allocateCommandPool(nullptr),
            allocator(nullptr) {

    }

    GraphicsSetupService::~GraphicsSetupService() {
        //TODO add assert to check uninit is called ? + check surface destroyed ?

        if (surface) { //TODO don't destroy with instance !
            vkDestroySurfaceKHR(vkInstance, surface, nullptr);
            surface = nullptr;
        }

        if (vkInstance) {
            vkDestroyInstance(vkInstance, nullptr);
            vkInstance = nullptr;
        }
    }

    bool GraphicsSetupService::isInitialized() const {
        return apiGraphicInitialized;
    }

    void GraphicsSetupService::initialize(const std::vector<std::string>& windowRequiredExtensions, const std::unique_ptr<SurfaceCreator>& surfaceCreator,
            FramebufferSizeRetriever& framebufferSizeRetriever) {
        this->framebufferSizeRetriever = &framebufferSizeRetriever;
        this->validationLayer = std::make_unique<ValidationLayer>();
        this->deviceHandler = std::make_unique<DeviceHandler>();

        createInstance(windowRequiredExtensions);
        validationLayer->initializeDebugMessenger(vkInstance);
        if (!surface) {
            surface = static_cast<VkSurfaceKHR>(surfaceCreator->createSurface(vkInstance));
        }
        deviceHandler->initializeDevices(vkInstance, surface);
        queueHandler.initializeQueueFamilies(deviceHandler->getPhysicalDevice(), surface);
        queueHandler.initializeQueues(deviceHandler->getLogicalDevice());

        createAllocateCommandPool();
        createAllocator();

        apiGraphicInitialized = true;
    }

    void GraphicsSetupService::uninitialize() {
        if (allocator) {
            vmaDestroyAllocator(allocator);
            allocator = nullptr;
        }

        if (apiGraphicInitialized) {
            vkDestroyCommandPool(getDevices().getLogicalDevice(), allocateCommandPool, nullptr);
            apiGraphicInitialized = false;
        }

        deviceHandler.reset();
        validationLayer.reset();
    }

    void GraphicsSetupService::createInstance(const std::vector<std::string>& windowRequiredExtensions) const {
        if (vkInstance) {
            return;
        }

        Logger::instance().logInfo("Create Vulkan instance with window required extensions: " + StringUtil::merge(windowRequiredExtensions, ','));

        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "VulkanApp-ff268de9-d92a-47b4-b6b0-8df93853565f";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Engine-04067ff4-d202-4e33-8ca0-c4f5e929affd";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = vulkanVersion;

        std::vector<std::string> requiredExtensionsString = VectorUtil::concatenate(windowRequiredExtensions, validationLayer->getRequiredExtensions());
        std::vector<const char*> requiredExtensions;
        requiredExtensions.reserve(requiredExtensionsString.size());
        for (const std::string& requiredExtensionString : requiredExtensionsString) {
            requiredExtensions.emplace_back(requiredExtensionString.data());
        }

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.empty() ? nullptr : requiredExtensions.data();
        validationLayer->initializeDebugMessengerForInstance(instanceCreateInfo);

        VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance);
        if (result != VK_SUCCESS) {
            throw UserAuthorityException("Failed to create Vulkan instance with error code: " + std::string(string_VkResult(result)), "Upgrade your graphic drivers to support Vulkan");
        }
    }

    const FramebufferSizeRetriever* GraphicsSetupService::getFramebufferSizeRetriever() const {
        return framebufferSizeRetriever;
    }

    VkSurfaceKHR GraphicsSetupService::getSurface() {
        assert(surface);
        return surface;
    }

    const DeviceHandler& GraphicsSetupService::getDevices() const {
        assert(deviceHandler);
        return *deviceHandler;
    }

    const QueueHandler& GraphicsSetupService::getQueues() const {
        assert(apiGraphicInitialized);
        return queueHandler;
    }

    VkCommandPool GraphicsSetupService::getAllocateCommandPool() const {
        assert(apiGraphicInitialized);
        return allocateCommandPool;
    }

    VmaAllocator GraphicsSetupService::getAllocator() const {
        assert(allocator);
        return allocator;
    }

    const ValidationLayer& GraphicsSetupService::getValidationLayer() const {
        assert(validationLayer);
        return *validationLayer;
    }

    void GraphicsSetupService::createAllocateCommandPool() {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueHandler.getGraphicsAndComputeQueueFamily();
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        VkResult result = vkCreateCommandPool(deviceHandler->getLogicalDevice(), &poolInfo, nullptr, &allocateCommandPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool with error code: " + std::string(string_VkResult(result)));
        }
    }

    void GraphicsSetupService::createAllocator() {
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.vulkanApiVersion = vulkanVersion;
        allocatorInfo.physicalDevice = deviceHandler->getPhysicalDevice();
        allocatorInfo.device = deviceHandler->getLogicalDevice();
        allocatorInfo.instance = vkInstance;
        allocatorInfo.flags = 0;
        if (deviceHandler->isMemoryBudgetExtSupported()) {
            allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        }

        vmaCreateAllocator(&allocatorInfo, &allocator);
    }

}
