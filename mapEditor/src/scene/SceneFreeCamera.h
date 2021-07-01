#pragma once

#include <QWidget>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

#include <widget/controller/mouse/MouseController.h>

namespace urchin {

    class SceneFreeCamera : public FreeCamera {
        public:
            SceneFreeCamera(float, float, float, const std::unique_ptr<MouseController>&);
            ~SceneFreeCamera() override = default;

            void loadCameraState(const std::string&);
            void saveCameraState(const std::string&) const;

            bool onMouseMove(double, double) override;

        private:
            static const char DATA_DELIMITER;

            double mouseXBeforeMove, mouseYBeforeMove;
            const std::unique_ptr<MouseController>& mouseController;
    };

}
