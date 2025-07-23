#include "scene/SceneWindowController.h"
#include "scene/SceneDisplayerWindow.h"

namespace urchin {

    QtSurfaceCreator::QtSurfaceCreator(SceneDisplayerWindow* window) :
            window(window) {
    }

    void* QtSurfaceCreator::createSurface(void* instance) {
        window->setupVkInstance(static_cast<VkInstance>(instance));
        return QVulkanInstance::surfaceForWindow(window);
    }

    QtFramebufferSizeRetriever::QtFramebufferSizeRetriever(SceneDisplayerWindow* window) :
            window(window) {
    }

    void QtFramebufferSizeRetriever::getFramebufferSizeInPixel(unsigned int& widthInPixel, unsigned int& heightInPixel) const {
        widthInPixel = 0;
        heightInPixel = 0;

        //When the window is minimized or Alt+Tabbed: "SDL_Vulkan_GetDrawableSize" return a valid size (2560x1440) but "VkSurfaceCapabilitiesKHR#currentExtent" contains a zero size (0x0)
        //Therefore, we return an invalid size here to avoid creating a swap chain with a wrong image extend
        if (!window->windowStates().testFlag(Qt::WindowMinimized)) {
            QSize windowSize = window->size() * window->devicePixelRatio();
            widthInPixel = windowSize.width() > 1 ? (unsigned int)windowSize.width() : 0;
            heightInPixel = windowSize.height() > 1 ? (unsigned int)windowSize.height() : 0;
        }
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

        std::vector<std::string> requiredExtensions;
        for (auto& extension : extensions) {
            bool extensionSupported = false;
            for (const auto& supportedExtension : QVulkanInstance().supportedExtensions()) {
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
