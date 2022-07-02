#include <cstring>
#include <UrchinCommon.h>

#include <graphics/api/vulkan/setup/handler/ValidationLayer.h>
using namespace urchin;

namespace urchin {

    //static
    std::vector<std::string> ValidationLayer::filterOutMessages;

    ValidationLayer::ValidationLayer() :
            validationLayer({"VK_LAYER_KHRONOS_validation"}),
            validationLevel(ConfigService::instance().getUnsignedIntValue("graphicsDebug.apiValidationLevel")),
            instance(nullptr),
            debugMessenger(nullptr),
            instanceDebugCreateInfo({}),
            features({}),
            featureEnables({}) {

        filterOutMessages.clear();
        filterOutMessages.emplace_back("vkQueuePresentKHR(): Returned error VK_ERROR_OUT_OF_DATE_KHR"); //error VK_ERROR_OUT_OF_DATE_KHR is handled by the application
        filterOutMessages.emplace_back("Attempting to enable extension VK_EXT_debug_utils"); //allow validation layer to be active for debug/development/production
        filterOutMessages.emplace_back("but only supports loader interface version 4"); //caused by wrong packaging of Ubuntu 21.10
    }

    void ValidationLayer::initializeDebugMessengerForInstance(VkInstanceCreateInfo& instanceCreateInfo) {
        if (validationLevel >= 2) {
            if (!checkValidationLayerSupport()) {
                throw std::runtime_error("Vulkan validation layers requested but not available");
            }
            instanceCreateInfo.enabledLayerCount = (uint32_t)validationLayer.size();
            instanceCreateInfo.ppEnabledLayerNames = validationLayer.data();
            populateDebugMessengerCreateInfo(instanceDebugCreateInfo);
            instanceCreateInfo.pNext = &instanceDebugCreateInfo;

            featureEnables[0] = VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT;
            featureEnables[1] = VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT;
            featureEnables[2] = VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT;
            featureEnables[3] = VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT;
            features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
            if (validationLevel >= 3) {
                features.enabledValidationFeatureCount = (uint32_t)featureEnables.size();
            } else {
                features.enabledValidationFeatureCount = 1;
            }
            features.pEnabledValidationFeatures = featureEnables.data();
            instanceDebugCreateInfo.pNext = &features;
        } else {
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.pNext = nullptr;
        }
    }

    void ValidationLayer::initializeDebugMessenger(VkInstance vkInstance) {
        if (validationLevel >= 2) {
            this->instance = vkInstance;

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            populateDebugMessengerCreateInfo(debugCreateInfo);

            VkResult result = createDebugUtilsMessengerEXT(vkInstance, &debugCreateInfo);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to set up debug messenger with error code: " + std::to_string(result));
            }
        }
    }

    void ValidationLayer::cleanup() {
        if (validationLevel >= 2 && instance) {
            destroyDebugUtilsMessengerEXT(instance);
        }
    }

    unsigned int ValidationLayer::getValidationLevel() const {
        return validationLevel;
    }

    std::vector<std::string> ValidationLayer::getRequiredExtensions() const {
        if (validationLevel >= 2) {
            return {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
        }
        return {};
    }

    bool ValidationLayer::checkValidationLayerSupport() const {
        uint32_t layerCount;
        VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        if (result != VK_SUCCESS && result != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to set up debug messenger with error code: " + std::to_string(result));
        }

        std::vector<VkLayerProperties> availableLayers(layerCount);
        result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        if (result != VK_SUCCESS && result != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to set up debug messenger with error code: " + std::to_string(result));
        }

        assert(validationLayer.size() == 1);
        return std::ranges::any_of(availableLayers, [&](const auto& layerProperties) {
            return std::strcmp(validationLayer[0], layerProperties.layerName) == 0;
        });
    }

    void ValidationLayer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const {
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
        auto validationMessage = std::string(pCallbackData->pMessage);
        if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            static unsigned int numErrorsLogged = 0;
            if (!ignoreValidationMessage(validationMessage) && numErrorsLogged < MAX_ERRORS_LOG) {
                Logger::instance().logError("Vulkan validation layer error: " + validationMessage);
                numErrorsLogged++;
            }
        } else {
            Logger::instance().logInfo("Vulkan validation layer info: " + validationMessage);
        }
        return VK_FALSE;
    }

    VkResult ValidationLayer::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, nullptr, &debugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void ValidationLayer::destroyDebugUtilsMessengerEXT(VkInstance instance) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, nullptr);
        }
    }

    bool ValidationLayer::ignoreValidationMessage(std::string_view validationMessage) {
        return std::ranges::any_of(filterOutMessages, [&validationMessage](const auto& filterOutMessage){
            return validationMessage.find(filterOutMessage) != std::string::npos;
        });
    }

}
