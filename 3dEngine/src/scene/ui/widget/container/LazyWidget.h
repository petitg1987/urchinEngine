#pragma once

#include "scene/ui/widget/Widget.h"

namespace urchin {

    /**
     * Widget which is able to lazy load its children.
     * Children loading/unloading is dependent of the LazyWidget's parent. Example: Container widget lazy load children only when they are visible on screen.
     */
    class LazyWidget final : public Widget {
        public:
            static std::shared_ptr<LazyWidget> create(Widget*, Position, Size, std::function<void(LazyWidget*)>);

            WidgetType getWidgetType() const override;

            void loadChildren();
            void unloadChildren();

        protected:
            void createOrUpdateWidget() override;

        private:
            LazyWidget(Position, Size, std::function<void(LazyWidget*)>);

            std::function<void(LazyWidget*)> loadChildrenFunction;
            bool isLoaded;
    };

}
