#pragma once

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>

namespace urchin {

    class CheckBox final : public Widget {
        public:
            static std::shared_ptr<CheckBox> create(Widget*, Position, Size, std::string);

            WidgetType getWidgetType() const override;

            void setChecked(bool);
            bool isChecked() const;

        private:
            CheckBox(Position, Size, std::string);

            void createOrUpdateWidget() override;
            bool requireRenderer() const override;

            std::shared_ptr<Texture> loadTexture(const UdaChunk*, std::string_view) const;
            void refreshTexture();
            bool onKeyReleaseEvent(InputDeviceKey) override;
            void switchValue();

            //properties
            const std::string skinName;

            //state
            bool bIsChecked;

            //visual
            std::shared_ptr<Texture> texChecked;
            std::shared_ptr<Texture> texUnchecked;
    };

}
