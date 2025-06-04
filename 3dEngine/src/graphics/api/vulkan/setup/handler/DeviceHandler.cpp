#include <cstring>
#include <cassert>
#include <ranges>
#include <libs/vkenum/vk_enum.h>
#include <UrchinCommon.h>

#include <graphics/api/vulkan/setup/handler/DeviceHandler.h>
#include <graphics/api/vulkan/setup/handler/QueueHandler.h>
#include <graphics/api/vulkan/render/handler/SwapChainHandler.h>
#include <graphics/render/GenericComputeBuilder.h>
using namespace urchin;

namespace urchin {

    PhysicalDeviceSuitability::PhysicalDeviceSuitability(VkPhysicalDevice physicalDevice, std::string deviceName, int score) :
            physicalDevice(physicalDevice),
            deviceName(std::move(deviceName)),
            score(score),
            missingRequisiteDescription(std::nullopt) {

    }

    PhysicalDeviceSuitability::PhysicalDeviceSuitability(VkPhysicalDevice physicalDevice, std::string missingRequisiteDescription) :
            physicalDevice(physicalDevice),
            deviceName(std::nullopt),
            score(-1),
            missingRequisiteDescription(missingRequisiteDescription) {

    }

    DeviceHandler::DeviceHandler() :
            devicesInitialized(false),
            surface(nullptr),
            physicalDevice(nullptr),
            logicalDevice(nullptr) {
        //List of features and extensions required to run the engine. Support percentage:
        // Vulkan 1.0: https://vulkan.gpuinfo.org/listfeaturescore10.php
        // Vulkan 1.2: https://vulkan.gpuinfo.org/listfeaturescore12.php
        // Vulkan 1.3: https://vulkan.gpuinfo.org/listfeaturescore13.php
        physicalDeviceExpectedFeatures = {
            PhysicalDeviceFeature{.offset = offsetof(VkPhysicalDeviceFeatures, independentBlend), .featureDescription = "independent blend", .optional = false}, //99.44%
            PhysicalDeviceFeature{.offset = offsetof(VkPhysicalDeviceFeatures, samplerAnisotropy), .featureDescription = "anisotropy", .optional = true} //91.34% (optional)
        };
        if (ConfigService::instance().getBoolValue("graphicsDebug.enableRobustBufferAccess")) {
            physicalDeviceExpectedFeatures.emplace_back(PhysicalDeviceFeature{.offset = offsetof(VkPhysicalDeviceFeatures, robustBufferAccess), .featureDescription = "robust buffer access", .optional = false}); //100%
        }
        physicalDeviceExpectedVulkan13Features = {
            PhysicalDeviceFeature{.offset = offsetof(VkPhysicalDeviceVulkan13Features, synchronization2), .featureDescription = "synchronization 2", .optional = false}, //99.88%
        };

        physicalDeviceRequiredExtensions = {
                std::make_pair<const char*, std::string>(VK_KHR_SWAPCHAIN_EXTENSION_NAME, "swap chain"),
        };
        physicalDeviceOptionalExtensions = {
                std::make_pair<const char*, std::string>(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME, "memory budget")
        };
    }

    DeviceHandler::~DeviceHandler() {
        if (logicalDevice) {
            vkDestroyDevice(logicalDevice, nullptr);
            logicalDevice = nullptr;
        }
    }

    void DeviceHandler::initializeDevices(VkInstance instance, VkSurfaceKHR vkSurface) {
        this->surface = vkSurface;

        physicalDevice = findPhysicalDevice(instance);
        logicalDevice = createLogicalDevice();

        devicesInitialized = true;
    }

    VkPhysicalDevice DeviceHandler::getPhysicalDevice() const {
        assert(devicesInitialized);
        return physicalDevice;
    }

    VkDevice DeviceHandler::getLogicalDevice() const {
        assert(devicesInitialized);
        return logicalDevice;
    }

    bool DeviceHandler::isFeature10Available(VkPhysicalDevice physicalDeviceToCheck, unsigned long featureOffset) const {
        VkPhysicalDeviceFeatures deviceFeatures{};
        vkGetPhysicalDeviceFeatures(physicalDeviceToCheck, &deviceFeatures);
        return *reinterpret_cast<VkBool32*>(((char*)&deviceFeatures) + featureOffset);
    }

    bool DeviceHandler::isFeature13Available(VkPhysicalDevice physicalDeviceToCheck, unsigned long featureOffset) const {
        VkPhysicalDeviceVulkan13Features deviceVulkan13Features{};
        deviceVulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        VkPhysicalDeviceFeatures2 deviceFeatures13{};
        deviceFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures13.pNext = &deviceVulkan13Features;
        vkGetPhysicalDeviceFeatures2(physicalDeviceToCheck, &deviceFeatures13);
        return *reinterpret_cast<VkBool32*>(((char*)&deviceVulkan13Features) + featureOffset);
    }

    bool DeviceHandler::isAnisotropySupported() const {
        assert(devicesInitialized);
        return isFeature10Available(physicalDevice, offsetof(VkPhysicalDeviceFeatures, samplerAnisotropy));
    }

    bool DeviceHandler::isMemoryBudgetExtSupported() const {
        assert(devicesInitialized);
        auto itPhysicsDeviceOptionalExt = physicalDeviceOptionalExtensionsSupported.find(physicalDevice);
        return itPhysicsDeviceOptionalExt != physicalDeviceOptionalExtensionsSupported.end() && std::ranges::find(itPhysicsDeviceOptionalExt->second, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME) != itPhysicsDeviceOptionalExt->second.end();
    }

    /**
     * Returns the most suitable physical device (=graphic card) to run the engine
     */
    VkPhysicalDevice DeviceHandler::findPhysicalDevice(VkInstance instance) {
        Logger::instance().logInfo("Finding a Vulkan physical device");

        uint32_t deviceCount = 0;
        VkResult resultEnumDevices = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (resultEnumDevices != VK_SUCCESS && resultEnumDevices != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to enumerate physics devices with error code: " + std::string(string_VkResult(resultEnumDevices)));
        }
        if (deviceCount == 0) {
            throw UserAuthorityException("Failed to find a graphic card with Vulkan support", "Upgrade your graphic drivers to support Vulkan");
        }

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        resultEnumDevices = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
        if (resultEnumDevices != VK_SUCCESS && resultEnumDevices != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to enumerate physics devices with error code: " + std::to_string(resultEnumDevices));
        }

        std::multimap<int, PhysicalDeviceSuitability> physicalDeviceCandidates;
        for (const auto& device : physicalDevices) {
            auto physicalDeviceSuitability = retrievePhysicalDeviceSuitability(device);
            physicalDeviceCandidates.insert(std::make_pair(physicalDeviceSuitability.score, physicalDeviceSuitability));
        }

        auto bestPhysicalDevice = physicalDeviceCandidates.rbegin();
        if (bestPhysicalDevice->first == -1) {
            assert(bestPhysicalDevice->second.missingRequisiteDescription.has_value());
            std::string noDeviceFoundReason = bestPhysicalDevice->second.missingRequisiteDescription.value();
            throw UserAuthorityException("Failed to find a suitable graphic card: " + noDeviceFoundReason, "Make sure your graphic card matches the minimum requirements and/or upgrade your graphic drivers");
        }

        assert(bestPhysicalDevice->second.deviceName.has_value());
        Logger::instance().logInfo("Physical device with a score of " + std::to_string(bestPhysicalDevice->second.score) + " found: " + bestPhysicalDevice->second.deviceName.value());

        return bestPhysicalDevice->second.physicalDevice;
    }

    PhysicalDeviceSuitability DeviceHandler::retrievePhysicalDeviceSuitability(VkPhysicalDevice physicalDeviceToCheck) {
        int score = 0;

        VkPhysicalDeviceProperties deviceProperties{};
        vkGetPhysicalDeviceProperties(physicalDeviceToCheck, &deviceProperties);

        //check expected features
        for (const auto& expectedFeature : physicalDeviceExpectedFeatures) {
            auto isFeatureAvailable = isFeature10Available(physicalDeviceToCheck, expectedFeature.offset);
            if (isFeatureAvailable) {
                score += 5000;
            } else if (!expectedFeature.optional) {
                return {physicalDeviceToCheck, "missing Vulkan feature '" + expectedFeature.featureDescription + "' support"};
            }
        }

        for (const auto& expectedFeature : physicalDeviceExpectedVulkan13Features) {
            auto isFeatureAvailable = isFeature13Available(physicalDeviceToCheck, expectedFeature.offset);
            if (isFeatureAvailable) {
                score += 5000;
            } else if (!expectedFeature.optional) {
                return {physicalDeviceToCheck, "missing Vulkan feature '" + expectedFeature.featureDescription + "' support"};
            }
        }

        //check required extensions
        for (const auto& [extensionName, extensionDescription] : physicalDeviceRequiredExtensions) {
            if (!checkPhysicalDeviceExtensionSupport(physicalDeviceToCheck, extensionName)) {
                return {physicalDeviceToCheck, "missing Vulkan extension '" + extensionDescription + "' support"};
            }
        }

        //check optional extensions
        for (const char* extensionName : std::views::keys(physicalDeviceOptionalExtensions)) {
            if (checkPhysicalDeviceExtensionSupport(physicalDeviceToCheck, extensionName)) {
                std::vector<const char*> emptyExt;
                physicalDeviceOptionalExtensionsSupported.try_emplace(physicalDeviceToCheck, emptyExt).first->second.emplace_back(extensionName);
            }
        }

        //check required queue families
        QueueHandler queueFamilyHandler;
        queueFamilyHandler.initializeQueueFamilies(physicalDeviceToCheck, surface);
        if (!queueFamilyHandler.isAllQueueFamiliesFound()) {
            return {physicalDeviceToCheck, "missing a queue family support"};
        }

        //check swap chain is adequate
        SwapChainSupportDetails swapChainSupport = SwapChainHandler::querySwapChainSupport(physicalDeviceToCheck);
        if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
            return {physicalDeviceToCheck, "missing adequate swap chain support"};
        }

        //check max compute work group invocations
        if (deviceProperties.limits.maxComputeWorkGroupInvocations < GenericComputeBuilder::MAX_COMPUTE_WORK_GROUP_INVOCATIONS) {
            std::string valueOverMin = std::to_string(deviceProperties.limits.maxComputeWorkGroupInvocations) + "/" + std::to_string(GenericComputeBuilder::MAX_COMPUTE_WORK_GROUP_INVOCATIONS);
            return {physicalDeviceToCheck, "minimum requirement for compute work group invocations is insufficient (" + valueOverMin + ")"};
        }

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { //discrete GPU have better performance
            score += 10000;
        }
        score += (int)deviceProperties.limits.maxImageDimension2D; //indicator of the device performance/quality
        return {physicalDeviceToCheck, std::string(deviceProperties.deviceName), score};
    }

    bool DeviceHandler::checkPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDeviceToCheck, const char* extensionName) const {
        uint32_t extensionCount;
        VkResult resultEnumExtension = vkEnumerateDeviceExtensionProperties(physicalDeviceToCheck, nullptr, &extensionCount, nullptr);
        if (resultEnumExtension != VK_SUCCESS && resultEnumExtension != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to enumerate device extension with error code: " + std::string(string_VkResult(resultEnumExtension)));
        }

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        resultEnumExtension = vkEnumerateDeviceExtensionProperties(physicalDeviceToCheck, nullptr, &extensionCount, availableExtensions.data());
        if (resultEnumExtension != VK_SUCCESS && resultEnumExtension != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to enumerate device extension with error code: " + std::string(string_VkResult(resultEnumExtension)));
        }

        return std::ranges::any_of(availableExtensions, [&extensionName](const auto& extension) {
            return std::strcmp(extension.extensionName, extensionName) == 0;
        });
    }

    VkDevice DeviceHandler::createLogicalDevice() {
        Logger::instance().logInfo("Creating a Vulkan logical device");

        QueueHandler queueFamilyHandler;
        queueFamilyHandler.initializeQueueFamilies(physicalDevice, surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set uniqueQueueFamilies = {queueFamilyHandler.getGraphicsAndComputeQueueFamily(), queueFamilyHandler.getPresentationQueueFamily()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        std::vector<const char*> physicalDeviceExtensions;
        std::ranges::for_each(physicalDeviceRequiredExtensions, [&physicalDeviceExtensions](std::pair<const char*, std::string>& ext) {
            physicalDeviceExtensions.emplace_back(ext.first);
        });
        std::ranges::for_each(physicalDeviceOptionalExtensions, [this, &physicalDeviceExtensions](std::pair<const char*, std::string>& ext) {
            auto itPhysicsDeviceOptionalExt = physicalDeviceOptionalExtensionsSupported.find(physicalDevice);
            if (itPhysicsDeviceOptionalExt != physicalDeviceOptionalExtensionsSupported.end() && std::ranges::find(itPhysicsDeviceOptionalExt->second, ext.first) != itPhysicsDeviceOptionalExt->second.end()) {
                physicalDeviceExtensions.emplace_back(ext.first);
            } else {
                const char* extensionName = ext.first;
                auto extDescriptionIt = std::ranges::find_if(physicalDeviceOptionalExtensions, [extensionName](const std::pair<const char*, std::string>& e) { return e.first == extensionName; });
                if (extDescriptionIt != physicalDeviceOptionalExtensions.end()) { //should be always true
                    Logger::instance().logInfo("Creating logical device without optional extension: " + extDescriptionIt->second);
                }
            }
        });

        VkPhysicalDeviceFeatures deviceVulkan10Features{};
        for (const auto& expectedFeature : physicalDeviceExpectedFeatures) {
            bool enableFeature = expectedFeature.optional ? isFeature10Available(physicalDevice, expectedFeature.offset) : true;
            *reinterpret_cast<VkBool32*>(((char*)&deviceVulkan10Features) + expectedFeature.offset) = enableFeature;
        }

        VkPhysicalDeviceVulkan13Features deviceVulkan13Features{};
        deviceVulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        for (const auto& expectedFeature : physicalDeviceExpectedVulkan13Features) {
            bool enableFeature = expectedFeature.optional ? isFeature13Available(physicalDevice, expectedFeature.offset) : true;
            *reinterpret_cast<VkBool32*>(((char*)&deviceVulkan13Features) + expectedFeature.offset) = enableFeature;
        }

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceVulkan10Features;
        createInfo.pNext = &deviceVulkan13Features;
        createInfo.enabledExtensionCount = (uint32_t)physicalDeviceExtensions.size();
        createInfo.ppEnabledExtensionNames = physicalDeviceExtensions.empty() ? nullptr : physicalDeviceExtensions.data();

        VkDevice device;
        VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device with error code: " + std::string(string_VkResult(result)));
        }
        return device;
    }

}
