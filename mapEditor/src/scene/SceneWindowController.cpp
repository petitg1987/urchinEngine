#include "SceneWindowController.h"

namespace urchin {

    QtSurfaceCreator::QtSurfaceCreator(SceneDisplayerWindow* window) :
            window(window) {
    }

    VkSurfaceKHR QtSurfaceCreator::createSurface(VkInstance instance) {
        vulkanInstance.setVkInstance(instance);
        if (!vulkanInstance.create()) {
            throw std::runtime_error("Failed to create Vulkan instance: " + std::to_string(vulkanInstance.errorCode()));
        }
        window->setVulkanInstance(&vulkanInstance); //TODO vulkanInstance never destroyed ? To store on window ?
        return QVulkanInstance::surfaceForWindow(window);
    }

    QtFramebufferSizeRetriever::QtFramebufferSizeRetriever(SceneDisplayerWindow* window) :
            window(window) {
    }

    void QtFramebufferSizeRetriever::getFramebufferSizeInPixel(int& widthInPixel, int& heightInPixel) const {
        QSize windowSize = window->size() * window->devicePixelRatio();
        widthInPixel = windowSize.width();
        heightInPixel = windowSize.height();
    }

    SceneWindowController::SceneWindowController(SceneDisplayerWindow* window) :
            window(window) {
    }

    std::vector<std::string> SceneWindowController::windowRequiredExtensions() {
        std::vector<std::string> requiredExtensions;
        for(auto& extension : QVulkanInstance().supportedExtensions()) { //TODO avoid deprecated extension
            requiredExtensions.emplace_back(std::string(extension.name.constData()));
        }
        return requiredExtensions;
    }

    std::unique_ptr<QtSurfaceCreator> SceneWindowController::getSurfaceCreator() const {
        return std::make_unique<QtSurfaceCreator>(window);
    }

    std::unique_ptr<QtFramebufferSizeRetriever> SceneWindowController::getFramebufferSizeRetriever() const {
        return std::make_unique<QtFramebufferSizeRetriever>(window);
    }

}
