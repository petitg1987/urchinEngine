#include "UrchinCommon.h"

#include "ValidationLayer.h"
using namespace urchin;

namespace urchin {

    //debug parameters
    bool DEBUG_DISPLAY_VULKAN_INFO = false;

    //static
    constexpr unsigned int ValidationLayer::MAX_ERRORS_LOG = 50;
    std::vector<std::string> ValidationLayer::FILTER_OUT_MESSAGES;

    ValidationLayer::ValidationLayer() :
            validationLayer({"VK_LAYER_KHRONOS_validation"}),
            bIsValidationActive(ConfigService::instance()->getBoolValue("graphics.validationLayerEnable")),
            instance(nullptr),
            debugMessenger(nullptr),
            instanceDebugCreateInfo({}),
            features({}),
            featureEnables({}) {

        FILTER_OUT_MESSAGES.clear();
        FILTER_OUT_MESSAGES.emplace_back("vkQueuePresentKHR(): Returned error VK_ERROR_OUT_OF_DATE_KHR"); //error VK_ERROR_OUT_OF_DATE_KHR is handled by the application
        FILTER_OUT_MESSAGES.emplace_back("Attempting to enable deprecated extension VK_EXT_debug_report"); //deprecated extension but still used by Renderdoc
        #ifndef NDEBUG
            FILTER_OUT_MESSAGES.emplace_back("Attempting to enable extension VK_EXT_debug_utils"); //allow validation layer to be active for debug/development
            FILTER_OUT_MESSAGES.emplace_back("Attempting to enable extension VK_EXT_debug_report"); //same as previous one but deprecated and still used by Renderdoc
        #endif
    }

    void ValidationLayer::initializeDebugMessengerForInstance(VkInstanceCreateInfo& instanceCreateInfo) {
        if (bIsValidationActive) {
            if (!checkValidationLayerSupport()) {
                throw std::runtime_error("Vulkan validation layers requested but not available");
            }
            instanceCreateInfo.enabledLayerCount = (uint32_t)validationLayer.size();
            instanceCreateInfo.ppEnabledLayerNames = validationLayer.data();
            populateDebugMessengerCreateInfo(instanceDebugCreateInfo);
            instanceCreateInfo.pNext = &instanceDebugCreateInfo;

            featureEnables[0] = VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT;
            features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
            features.enabledValidationFeatureCount = 1;
            features.pEnabledValidationFeatures = featureEnables.data();
            instanceDebugCreateInfo.pNext = &features;
        } else {
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.pNext = nullptr;
        }
    }

    void ValidationLayer::initializeDebugMessenger(VkInstance vkInstance) {
        if (bIsValidationActive) {
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
        if (bIsValidationActive && instance) {
            destroyDebugUtilsMessengerEXT(instance);
        }
    }

    bool ValidationLayer::isValidationActive() const {
        return bIsValidationActive;
    }

    std::vector<std::string> ValidationLayer::getRequiredExtensions() const {
        if (bIsValidationActive) {
            return {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
        }
        return {};
    }

    bool ValidationLayer::checkValidationLayerSupport() const {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        assert(validationLayer.size() == 1);
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(validationLayer[0], layerProperties.layerName) == 0) {
                return true;
            }
        }
        return false;
    }

    void ValidationLayer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        if(DEBUG_DISPLAY_VULKAN_INFO) {
            createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        }
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
        std::string validationMessage = std::string(pCallbackData->pMessage);
        if(severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            static unsigned int numErrorsLogged = 0;
            if(!ignoreValidationMessage(validationMessage) && numErrorsLogged++ < MAX_ERRORS_LOG) {
                Logger::instance()->logError("Vulkan validation layer error: " + validationMessage);
            }
        } else {
            Logger::instance()->logInfo("Vulkan validation layer info: " + validationMessage);
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

    bool ValidationLayer::ignoreValidationMessage(const std::string& validationMessage) {
        return std::any_of(FILTER_OUT_MESSAGES.begin(), FILTER_OUT_MESSAGES.end(), [&validationMessage](const auto& filterOutMessage){
            return validationMessage.find(filterOutMessage) != std::string::npos;
        });
    }

}
