#pragma once

#include <vector>
#include <memory>
#include <QVulkanInstance>
#include <Urchin3dEngine.h>

namespace urchin {

    class SceneDisplayerWindow;

    class QtSurfaceCreator final : public urchin::SurfaceCreator {
        public:
            explicit QtSurfaceCreator(SceneDisplayerWindow*);
            void* createSurface(void*) override;

        private:
            SceneDisplayerWindow *window;
    };

    class QtFramebufferSizeRetriever final : public urchin::FramebufferSizeRetriever {
        public:
            explicit QtFramebufferSizeRetriever(SceneDisplayerWindow*);
            void getFramebufferSizeInPixel(unsigned int&, unsigned int&) const override;

        private:
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
