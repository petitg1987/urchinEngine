#pragma once

#include <vulkan/vulkan.h>

namespace urchin {

    class ValidationLayer {
        public:
            ValidationLayer();
            ~ValidationLayer();

            void initializeDebugMessengerForInstance(VkInstanceCreateInfo&);
            void initializeDebugMessenger(VkInstance);

            unsigned int getValidationLevel() const;
            std::vector<std::string> getRequiredExtensions() const;

        private:
            bool checkValidationLayerSupport() const;

            void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;
            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
            VkResult createDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*);
            void destroyDebugUtilsMessengerEXT(VkInstance) const;
            static bool ignoreValidationMessage(std::string_view);

            static constexpr unsigned int MAX_ERRORS_LOG = 50;
            static std::vector<std::string> filterOutMessages;
            std::vector<const char*> validationLayer;

            unsigned int validationLevel;

            VkInstance instance;
            VkDebugUtilsMessengerEXT debugMessenger;
            VkDebugUtilsMessengerCreateInfoEXT instanceDebugCreateInfo;

            VkValidationFeaturesEXT features;
            std::array<VkValidationFeatureEnableEXT, 4> featureEnables;
    };

}
