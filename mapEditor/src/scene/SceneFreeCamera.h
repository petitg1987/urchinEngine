#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

#include <widget/controller/mouse/MouseController.h>

namespace urchin {

    class SceneFreeCamera final : public FreeCamera {
        public:
            SceneFreeCamera(float, float, float, MouseController&);
            ~SceneFreeCamera() override = default;

            void loadCameraState(const std::string&);
            void saveCameraState(const std::string&) const;

            bool onMouseMove(double, double) override;

        private:
            static constexpr char DATA_DELIMITER = '@';

            bool disableNextMouseEvent;
            MouseController& mouseController;
    };

}
