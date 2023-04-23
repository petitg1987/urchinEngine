#include <scene/ui/displayer/WidgetDisplayable.h>
#include <scene/ui/displayer/WidgetInstanceDisplayer.h>

namespace urchin {

    WidgetDisplayable::~WidgetDisplayable() {
        assert(widgetInstanceDisplayers.empty()); //can not remove a model when displayer still use it
    }

    void WidgetDisplayable::attachWidgetInstanceDisplayer(WidgetInstanceDisplayer& widgetInstanceDisplayer) {
        widgetInstanceDisplayers.push_back(&widgetInstanceDisplayer);
    }

    void WidgetDisplayable::detachWidgetInstanceDisplayer(WidgetInstanceDisplayer& widgetInstanceDisplayer) {
        std::size_t erasedCount = std::erase(widgetInstanceDisplayers, &widgetInstanceDisplayer);
        if (erasedCount != 1) {
            throw std::runtime_error("Removing the model displayer from model fail");
        }
    }

    std::span<WidgetInstanceDisplayer* const> WidgetDisplayable::getWidgetInstanceDisplayers() const {
        return widgetInstanceDisplayers;
    }

    std::size_t WidgetDisplayable::computeInstanceId() const {
        const auto* widget = static_cast<const Widget*>(this);

        if (widget->isScissorEnabled()) {
            return INSTANCING_DENY_ID; //no instancing
        }

        std::size_t implementationInstanceId = getImplementationInstanceId();
        if (implementationInstanceId == INSTANCING_DENY_ID) {
            return INSTANCING_DENY_ID; //no instancing
        }

        std::size_t instanceId = implementationInstanceId;

        //add ID for vertexCoord
        HashUtil::combine(instanceId, widget->getParent());
        HashUtil::combine(instanceId, widget->getSize().getHeight(), widget->getSize().getHeightType(), widget->getSize().getWidth(), widget->getSize().getWidthType());

        //TODO add id for uiRenderer ?

        return instanceId;
    }

    std::size_t WidgetDisplayable::getImplementationInstanceId() const {
        //no instancing by default. Most of the time, widgets cannot be easily instanced due to their dynamic aspect (button focus/un-focused, checkbox checked/un-checked...)
        return INSTANCING_DENY_ID;
    }

}

