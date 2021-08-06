#pragma once

#include <vector>

#include <scene/ui/widget/Widget.h>

namespace urchin {

    //TODO add comment here ...
    class LazyWidget : public Widget {
        public:
            static std::shared_ptr<LazyWidget> create(Widget*, Position, Size);

            void setupLazyChildren(std::function<void()>);

            void loadChildren();
            void unloadChildren();

        protected:
            void createOrUpdateWidget() override;

        private:
            LazyWidget(Position, Size);

            std::function<void()> loadChildrenFunction;
            bool isLoaded;
    };

}
