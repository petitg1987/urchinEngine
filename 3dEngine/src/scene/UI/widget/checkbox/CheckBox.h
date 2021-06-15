#pragma once

#include <scene/UI/widget/Widget.h>
#include <scene/UI/widget/Position.h>
#include <scene/UI/widget/Size.h>

namespace urchin {

    class CheckBox : public Widget {
        public:
            CheckBox(Widget*, Position, Size, std::string);

            bool isChecked() const;

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

        private:
            std::shared_ptr<Texture> loadTexture(const std::shared_ptr<XmlChunk>&, const std::string&) const;
            bool onKeyReleaseEvent(unsigned int) override;

            //properties
            const std::string nameSkin;

            //state
            bool bIsChecked;

            //visual
            std::shared_ptr<Texture> texChecked, texUnchecked;
            std::shared_ptr<GenericRenderer> checkBoxRenderer;
    };

}
