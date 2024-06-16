#pragma once

#include <optional>
#include <string>
#include <vulkan/vulkan.h>

namespace urchin {

    struct PhysicalDeviceFeature {
        unsigned long offset;
        std::string featureDescription;
        bool optional;
    };

    struct PhysicalDeviceSuitability {
        PhysicalDeviceSuitability(VkPhysicalDevice, int);
        PhysicalDeviceSuitability(VkPhysicalDevice, std::string);

        VkPhysicalDevice physicalDevice;
        int score;
        std::optional<std::string> missingRequisiteDescription;
    };

    class DeviceHandler {
        public:
            DeviceHandler();
            DeviceHandler(const DeviceHandler&) = delete;

            void initializeDevices(VkInstance instance, VkSurfaceKHR);
            void cleanup();

            VkPhysicalDevice getPhysicalDevice() const;
            VkDevice getLogicalDevice() const;

            bool isAnisotropySupported() const;
            bool isMemoryBudgetExtSupported() const;

        private:
            VkPhysicalDevice findPhysicalDevice(VkInstance instance);
            PhysicalDeviceSuitability retrievePhysicalDeviceSuitability(VkPhysicalDevice);
            bool checkPhysicalDeviceExtensionSupport(VkPhysicalDevice, const char*) const;

            VkDevice createLogicalDevice();

            std::vector<PhysicalDeviceFeature> physicalDeviceExpectedFeatures;
            std::vector<PhysicalDeviceFeature> physicalDeviceExpectedVulkan12Features;
            std::vector<PhysicalDeviceFeature> physicalDeviceExpectedVulkan13Features;
            std::vector<std::pair<const char*, std::string>> physicalDeviceRequiredExtensions;
            std::vector<std::pair<const char*, std::string>> physicalDeviceOptionalExtensions;
            std::map<VkPhysicalDevice, std::vector<const char*>> physicalDeviceOptionalExtensionsSupported;
            bool devicesInitialized;
            VkSurfaceKHR surface;

            VkPhysicalDevice physicalDevice;

            VkDevice logicalDevice;
    };

}
