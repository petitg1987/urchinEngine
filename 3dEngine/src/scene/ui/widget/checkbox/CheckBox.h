#pragma once

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>

namespace urchin {

    class CheckBox : public Widget {
        public:
            static std::shared_ptr<CheckBox> create(Widget*, Position, Size, std::string);

            WidgetType getWidgetType() const override;

            void setChecked(bool);
            bool isChecked() const;

        protected:
            void createOrUpdateWidget() override;

        private:
            CheckBox(Position, Size, std::string);

            std::shared_ptr<Texture> loadTexture(const UdaChunk*, std::string_view) const;
            void refreshTexture() const;
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
