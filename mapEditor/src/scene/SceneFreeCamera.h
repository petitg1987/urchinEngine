#ifndef URCHINENGINE_SCENEFREECAMERA_H
#define URCHINENGINE_SCENEFREECAMERA_H

#include <QWidget>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

#include <widget/controller/mouse/MouseController.h>

namespace urchin {

    class SceneFreeCamera : public FreeCamera {
        public:
            SceneFreeCamera(float, float, float, const std::unique_ptr<MouseController>&);
            ~SceneFreeCamera() override = default;

            void moveMouse(unsigned int, unsigned int) override;

            void loadCameraState(const std::string&);
            void saveCameraState(const std::string&) const;

        private:
            static const char DATA_DELIMITER;

            const std::unique_ptr<MouseController>& mouseController;
    };

}

#endif
