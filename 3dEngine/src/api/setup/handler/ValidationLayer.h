#pragma once

#include <vulkan/vulkan.h>

namespace urchin {

    class ValidationLayer {
        public:
            ValidationLayer();

            void initializeDebugMessengerForInstance(VkInstanceCreateInfo&);
            void initializeDebugMessenger(VkInstance);
            void cleanup();

            bool isValidationActive() const;
            std::vector<std::string> getRequiredExtensions() const;

        private:
            bool checkValidationLayerSupport() const;

            void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;
            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
            VkResult createDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*);
            void destroyDebugUtilsMessengerEXT(VkInstance);
            static bool ignoreValidationMessage(std::string_view);

            static constexpr unsigned int MAX_ERRORS_LOG = 50;
            static std::vector<std::string> filterOutMessages;
            std::vector<const char*> validationLayer;

            bool bIsValidationActive;

            VkInstance instance;
            VkDebugUtilsMessengerEXT debugMessenger;
            VkDebugUtilsMessengerCreateInfoEXT instanceDebugCreateInfo;

            VkValidationFeaturesEXT features;
            std::array<VkValidationFeatureEnableEXT, 1> featureEnables;
    };

}
