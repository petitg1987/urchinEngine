#pragma once

#include <vector>

namespace urchin {

    class WidgetInstanceDisplayer;

    class WidgetDisplayable {
        public:
            static constexpr std::size_t INSTANCING_DENY_ID = 0;

            virtual ~WidgetDisplayable();

            void attachWidgetInstanceDisplayer(WidgetInstanceDisplayer&);
            void detachWidgetInstanceDisplayer(WidgetInstanceDisplayer&);
            std::span<WidgetInstanceDisplayer* const> getWidgetInstanceDisplayers() const;

            std::size_t computeInstanceId() const;
            virtual std::size_t getImplementationInstanceId() const;

        private:
            std::vector<WidgetInstanceDisplayer*> widgetInstanceDisplayers;
    };

}
