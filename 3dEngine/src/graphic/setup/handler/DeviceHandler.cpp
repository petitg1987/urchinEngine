#include <UrchinCommon.h>

#include <graphic/setup/handler/DeviceHandler.h>
#include <graphic/setup/handler/QueueHandler.h>
#include <graphic/render/handler/SwapChainHandler.h>
using namespace urchin;

namespace urchin {

    PhysicalDeviceFeature::PhysicalDeviceFeature(unsigned long offset, std::string featureDescription) :
            offset(offset),
            featureDescription(std::move(featureDescription)) {

    }

    PhysicalDeviceSuitability::PhysicalDeviceSuitability(VkPhysicalDevice physicalDevice, int score) :
            physicalDevice(physicalDevice),
            score(score) {

    }

    PhysicalDeviceSuitability::PhysicalDeviceSuitability(VkPhysicalDevice physicalDevice, std::string missingRequisiteDescription) :
            physicalDevice(physicalDevice),
            score(-1),
            missingRequisiteDescription(missingRequisiteDescription) {

    }

    DeviceHandler::DeviceHandler() :
            devicesInitialized(false),
            surface(nullptr),
            physicalDevice(nullptr),
            logicalDevice(nullptr) {
        //List of features and extensions required to run the engine:
        physicalDeviceRequiredFeatures = {
                PhysicalDeviceFeature(offsetof(VkPhysicalDeviceFeatures, geometryShader), "geometry shader"),
                PhysicalDeviceFeature(offsetof(VkPhysicalDeviceFeatures, wideLines), "wide lines"),
                PhysicalDeviceFeature(offsetof(VkPhysicalDeviceFeatures, fillModeNonSolid), "file mode non solid"),
                PhysicalDeviceFeature(offsetof(VkPhysicalDeviceFeatures, samplerAnisotropy), "anisotropy")
        };
        physicalDeviceRequiredExtensions = {
                std::make_pair<const char*, std::string>(VK_KHR_SWAPCHAIN_EXTENSION_NAME, "swap chain")
        };
    }

    void DeviceHandler::initializeDevices(VkInstance instance, VkSurfaceKHR vkSurface) {
        this->surface = vkSurface;

        physicalDevice = findPhysicalDevice(instance);
        logicalDevice = createLogicalDevice();

        devicesInitialized = true;
    }

    void DeviceHandler::cleanup() {
        if (logicalDevice) {
            vkDestroyDevice(logicalDevice, nullptr);
        }
    }

    VkPhysicalDevice DeviceHandler::getPhysicalDevice() const {
        assert(devicesInitialized);
        return physicalDevice;
    }

    VkDevice DeviceHandler::getLogicalDevice() const {
        assert(devicesInitialized);
        return logicalDevice;
    }

    /**
     * Returns the most suitable physical device (=graphic card) to run the engine
     */
    VkPhysicalDevice DeviceHandler::findPhysicalDevice(VkInstance instance) {
        Logger::instance()->logInfo("Find Vulkan physical device");

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw UserAuthorityException("Failed to find a graphic card with Vulkan support", "Upgrade your graphic drivers to support Vulkan");
        }

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

        std::multimap<int, PhysicalDeviceSuitability> physicalDeviceCandidates;
        for (const auto& device : physicalDevices) {
            auto physicalDeviceSuitability = retrievePhysicalDeviceSuitability(device);
            physicalDeviceCandidates.insert(std::make_pair(physicalDeviceSuitability.score, physicalDeviceSuitability));
        }

        auto bestPhysicalDevice = physicalDeviceCandidates.rbegin();
        if (bestPhysicalDevice->first == -1) {
            assert(bestPhysicalDevice->second.missingRequisiteDescription.has_value());
            std::string noDeviceFoundReason = bestPhysicalDevice->second.missingRequisiteDescription.value();
            throw UserAuthorityException("Failed to find a suitable graphic card: " + noDeviceFoundReason, "Make sure your graphic card matches the minimum requirement");
        }

        return bestPhysicalDevice->second.physicalDevice;
    }

    PhysicalDeviceSuitability DeviceHandler::retrievePhysicalDeviceSuitability(VkPhysicalDevice physicalDeviceToCheck) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDeviceToCheck, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDeviceToCheck, &deviceFeatures);

        //check required features
        for (const auto& requiredFeature : physicalDeviceRequiredFeatures) {
            auto isFeatureAvailable = *reinterpret_cast<VkBool32*>(((char *)&deviceFeatures) + requiredFeature.offset);
            if (!isFeatureAvailable) {
                return PhysicalDeviceSuitability(physicalDeviceToCheck, "missing " + requiredFeature.featureDescription + " support");
            }
        }

        //check required extensions
        for (const auto& requiredExtension : physicalDeviceRequiredExtensions) {
            if (!checkPhysicalDeviceExtensionSupport(physicalDeviceToCheck, requiredExtension.first)) {
                return PhysicalDeviceSuitability(physicalDeviceToCheck, "missing " + requiredExtension.second + " support");
            }
        }

        //check swap chain is adequate
        SwapChainSupportDetails swapChainSupport = SwapChainHandler::querySwapChainSupport(physicalDeviceToCheck);
        if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
            return PhysicalDeviceSuitability(physicalDeviceToCheck, "missing adequate swap chain support");
        }

        //check required queue families
        QueueHandler queueFamilyHandler;
        queueFamilyHandler.initializeQueueFamilies(physicalDeviceToCheck, surface);
        if (!queueFamilyHandler.isAllQueueFamiliesFound()) {
            return PhysicalDeviceSuitability(physicalDeviceToCheck, "missing a queue family support");
        }

        int score = 0;
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { //discrete GPU have better performance
            score += 10000;
        }
        score += (int)deviceProperties.limits.maxImageDimension2D; //indicator of the device performance/quality
        return PhysicalDeviceSuitability(physicalDeviceToCheck, score);
    }

    bool DeviceHandler::checkPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDeviceToCheck, const char* extensionName) const {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDeviceToCheck, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDeviceToCheck, nullptr, &extensionCount, availableExtensions.data());

        for (const auto& extension : availableExtensions) {
            if (strcmp(extension.extensionName, extensionName) == 0) {
                return true;
            }
        }

        return false;
    }

    VkDevice DeviceHandler::createLogicalDevice() {
        Logger::instance()->logInfo("Create Vulkan logical device");

        QueueHandler queueFamilyHandler;
        queueFamilyHandler.initializeQueueFamilies(physicalDevice, surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {queueFamilyHandler.getGraphicsQueueFamily(), queueFamilyHandler.getPresentationQueueFamily()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        for (const auto& requiredFeature : physicalDeviceRequiredFeatures) {
            *reinterpret_cast<VkBool32*>(((char *)&deviceFeatures) + requiredFeature.offset) = true;
        }

        std::vector<const char*> physicalDeviceExtensions;
        std::for_each(physicalDeviceRequiredExtensions.begin(), physicalDeviceRequiredExtensions.end(), [&](std::pair<const char*, std::string>& ext) {physicalDeviceExtensions.emplace_back(ext.first);});
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = (uint32_t)physicalDeviceExtensions.size();
        createInfo.ppEnabledExtensionNames = physicalDeviceExtensions.data();

        VkDevice device;
        VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device with error code: " + std::to_string(result));
        }
        return device;
    }

}
