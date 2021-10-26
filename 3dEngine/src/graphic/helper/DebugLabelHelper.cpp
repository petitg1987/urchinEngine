#include <cstring>

#include <graphic/helper/DebugLabelHelper.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    void DebugLabelHelper::nameObject(ObjectType objectType, void* object, const std::string& objectName) {
        if (GraphicService::instance().getValidationLayer().isValidationActive()) {
            auto logicalDevice = GraphicService::instance().getDevices().getLogicalDevice();

            auto pfnSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(logicalDevice, "vkSetDebugUtilsObjectNameEXT"));
            auto objectTypeInfo = toVkObjectType(objectType);
            auto objectFullName = objectTypeInfo.second + objectName;

            VkDebugUtilsObjectNameInfoEXT objectNameInfo{};
            objectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            objectNameInfo.objectType = objectTypeInfo.first;
            objectNameInfo.objectHandle = (uint64_t) object;
            objectNameInfo.pObjectName = objectFullName.c_str();
            pfnSetDebugUtilsObjectNameEXT(GraphicService::instance().getDevices().getLogicalDevice(), &objectNameInfo);
        }
    }

    void DebugLabelHelper::beginDebugRegion(VkCommandBuffer commandBuffer, const std::string& labelName, const Vector4<float>& color) {
        if (GraphicService::instance().getValidationLayer().isValidationActive()) {
            auto pfnCmdBeginDebugUtilsLabel = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetDeviceProcAddr(GraphicService::instance().getDevices().getLogicalDevice(), "vkCmdBeginDebugUtilsLabelEXT"));

            VkDebugUtilsLabelEXT markerInfo{};
            markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            std::memcpy(markerInfo.color, &color[0], sizeof(Vector4<float>));
            markerInfo.pLabelName = labelName.c_str();

            pfnCmdBeginDebugUtilsLabel(commandBuffer, &markerInfo);
        }
    }

    void DebugLabelHelper::endDebugRegion(VkCommandBuffer commandBuffer) {
        if (GraphicService::instance().getValidationLayer().isValidationActive()) {
            auto pfnCmdEndDebugUtilsLabel = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetDeviceProcAddr(GraphicService::instance().getDevices().getLogicalDevice(), "vkCmdEndDebugUtilsLabelEXT"));

            pfnCmdEndDebugUtilsLabel(commandBuffer);
        }
    }

    std::pair<VkObjectType, std::string> DebugLabelHelper::toVkObjectType(ObjectType objectType) {
        if (objectType == PIPELINE) {
            return std::make_pair(VK_OBJECT_TYPE_PIPELINE, "[PIPELINE] ");
        } else if (objectType == RENDER_PASS) {
            return std::make_pair(VK_OBJECT_TYPE_RENDER_PASS, "[RENDER_PASS] ");
        } else if (objectType == SHADER) {
            return std::make_pair(VK_OBJECT_TYPE_SHADER_MODULE, "[SHADER] ");
        }
        throw std::runtime_error("Unknown graphic object type: " + std::to_string(objectType));
    }

}
