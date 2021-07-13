#include <scene/SceneWindowController.h>

namespace urchin {

    QtSurfaceCreator::QtSurfaceCreator(SceneDisplayerWindow* window) :
            window(window) {
    }

    VkSurfaceKHR QtSurfaceCreator::createSurface(VkInstance instance) {
        window->setupVkInstance(instance);
        return QVulkanInstance::surfaceForWindow(window);
    }

    QtFramebufferSizeRetriever::QtFramebufferSizeRetriever(SceneDisplayerWindow* window) :
            window(window) {
    }

    void QtFramebufferSizeRetriever::getFramebufferSizeInPixel(unsigned int& widthInPixel, unsigned int& heightInPixel) const {
        QSize windowSize = window->size() * window->devicePixelRatio();
        widthInPixel = (unsigned int)windowSize.width();
        heightInPixel = (unsigned int)windowSize.height();
    }

    SceneWindowController::SceneWindowController(SceneDisplayerWindow* window) :
            window(window) {
    }

    std::vector<std::string> SceneWindowController::windowRequiredExtensions() {
        std::vector<std::string> extensions;
        extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
        extensions.emplace_back("VK_KHR_xcb_surface"); //Unix
        extensions.emplace_back("VK_KHR_xlib_surface"); //Unix
        extensions.emplace_back("VK_KHR_wayland_surface"); //Unix
        extensions.emplace_back("VK_KHR_win32_surface"); //Windows
        extensions.emplace_back("VK_EXT_metal_surface"); //Mac OS
        extensions.emplace_back("VK_KHR_android_surface"); //Android
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        std::vector<std::string> requiredExtensions;
        for (auto& extension : extensions) {
            bool extensionSupported = false;
            for (auto& supportedExtension : QVulkanInstance().supportedExtensions()) {
                if (extension == std::string(supportedExtension.name.constData())) {
                    extensionSupported = true;
                    break;
                }
            }

            if (extensionSupported) {
                requiredExtensions.emplace_back(extension);
            }
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
