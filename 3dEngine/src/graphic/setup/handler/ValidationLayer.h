#ifndef URCHINENGINE_VALIDATIONLAYER_H
#define URCHINENGINE_VALIDATIONLAYER_H

#include <vulkan/vulkan.h>

namespace urchin {

    class ValidationLayer {
        public:
            ValidationLayer();

            void initializeDebugMessengerForInstance(VkInstanceCreateInfo&);
            void initializeDebugMessenger(VkInstance);
            void cleanup();

            bool isValidationActive() const;
            std::vector<const char*> getRequiredExtensions() const;

        private:
            bool checkValidationLayerSupport() const;

            static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
            VkResult createDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*);
            void destroyDebugUtilsMessengerEXT(VkInstance);
            static bool ignoreValidationMessage(const std::string&);

            static const unsigned int MAX_ERRORS_LOG;
            static std::vector<std::string> FILTER_OUT_MESSAGES;
            std::vector<const char*> validationLayer;

            bool bIsValidationActive;

            VkInstance instance;
            VkDebugUtilsMessengerEXT debugMessenger;
            VkDebugUtilsMessengerCreateInfoEXT instanceDebugCreateInfo;

            VkValidationFeaturesEXT features;
            std::array<VkValidationFeatureEnableEXT, 1> featureEnables;
    };

}

#endif
