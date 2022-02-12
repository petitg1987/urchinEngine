#pragma once

#include <vector>
#include <memory>
#include <QVulkanInstance>

namespace urchin {

    class SceneDisplayerWindow;

    struct QtSurfaceCreator : public urchin::SurfaceCreator {
        explicit QtSurfaceCreator(SceneDisplayerWindow*);
        VkSurfaceKHR createSurface(VkInstance instance) override;

        SceneDisplayerWindow *window;
    };

    struct QtFramebufferSizeRetriever : public urchin::FramebufferSizeRetriever {
        explicit QtFramebufferSizeRetriever(SceneDisplayerWindow*);
        void getFramebufferSizeInPixel(unsigned int&, unsigned int&) const override;

        SceneDisplayerWindow *window;
    };

    class SceneWindowController {
        public:
            explicit SceneWindowController(SceneDisplayerWindow*);

            static std::vector<std::string> windowRequiredExtensions();
            std::unique_ptr<QtSurfaceCreator> getSurfaceCreator() const;
            std::unique_ptr<QtFramebufferSizeRetriever> getFramebufferSizeRetriever() const;

        private:
            SceneDisplayerWindow *window;
    };

}
