#include <cstring>
#include <libs/vkenum/vk_enum.h>
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
        filterOutMessages.emplace_back("vkCreateInstance(): Both GPU Assisted Validation and Normal Core Check Validation are enabled"); //allow validation layer to be active for debug
        filterOutMessages.emplace_back("vkCreateInstance(): Attempting to enable extension VK_EXT_debug_utils"); //allow validation layer to be active for debug
        filterOutMessages.emplace_back("vkCreateDevice(): Internal Warning:"); //internal warning
    }

    ValidationLayer::~ValidationLayer() {
        if (validationLevel >= 2 && instance) {
            destroyDebugUtilsMessengerEXT(instance);
        }
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
                throw std::runtime_error("Failed to set up debug messenger with error code: " + std::string(string_VkResult(result)));
            }
        }
    }

    unsigned int ValidationLayer::getValidationLevel() const {
        return validationLevel;
    }

    std::vector<std::string> ValidationLayer::getRequiredExtensions() const {
        if (validationLevel >= 1) {
            return {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
        }
        return {};
    }

    bool ValidationLayer::checkValidationLayerSupport() const {
        uint32_t layerCount;
        VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        if (result != VK_SUCCESS && result != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to set up debug messenger with error code: " + std::string(string_VkResult(result)));
        }

        std::vector<VkLayerProperties> availableLayers(layerCount);
        result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        if (result != VK_SUCCESS && result != VK_INCOMPLETE) {
            throw std::runtime_error("Failed to set up debug messenger with error code: " + std::string(string_VkResult(result)));
        }

        assert(validationLayer.size() == 1);
        return std::ranges::any_of(availableLayers, [this](const auto& layerProperties) {
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
        if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT && !ignoreValidationMessage(validationMessage)) {
            std::string errorMessage = "Vulkan validation layer error: " + validationMessage;

            #ifdef URCHIN_DEBUG
                std::cerr << errorMessage << std::endl;
            #endif

            static unsigned int numErrorsLogged = 0;
            if (numErrorsLogged < MAX_ERRORS_LOG) {
                Logger::instance().logError(errorMessage);
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

    void ValidationLayer::destroyDebugUtilsMessengerEXT(VkInstance instance) const {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, nullptr);
        }
    }

    bool ValidationLayer::ignoreValidationMessage(std::string_view validationMessage) {
        return std::ranges::any_of(filterOutMessages, [&validationMessage](const auto& filterOutMessage) {
            return validationMessage.find(filterOutMessage) != std::string::npos;
        });
    }

}
