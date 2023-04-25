#include <scene/ui/displayer/WidgetSetDisplayer.h>
#include <scene/ui/displayer/WidgetInstanceDisplayer.h>

namespace urchin {

    WidgetSetDisplayer::WidgetSetDisplayer(const UIRenderer& uiRenderer) :
            uiRenderer(uiRenderer) {

    }

    WidgetSetDisplayer::~WidgetSetDisplayer() {
        clearDisplayers();
    }

    WidgetInstanceDisplayer* WidgetSetDisplayer::findWidgetInstanceDisplayer(const Widget& widget) const {
        for (WidgetInstanceDisplayer* widgetInstanceDisplayer : widget.getWidgetInstanceDisplayers()) {
            if (&widgetInstanceDisplayer->getWidgetSetDisplayer() == this) {
                return widgetInstanceDisplayer;
            }
        }
        return nullptr;
    }

    void WidgetSetDisplayer::clearDisplayers() {
        widgetDisplayers.clear();
        widgetInstanceDisplayers.clear();
    }

    void WidgetSetDisplayer::removeWidgetFromDisplayer(Widget& widget, WidgetInstanceDisplayer& widgetInstanceDisplayer) const {
        widgetInstanceDisplayer.removeInstanceWidget(widget);
        if (widgetInstanceDisplayer.getInstanceCount() == 0) {
            //to do:
            // - remove the displayer based on strategy to define (unused for x seconds, too much unused displayers, etc.)
            // - erase displayer from widgetDisplayers/widgetInstanceDisplayers
        }
    }

    void WidgetSetDisplayer::addWidgetToDisplayer(Widget& widget, WidgetInstanceDisplayer& widgetInstanceDisplayer) const {
        widgetInstanceDisplayer.addInstanceWidget(widget);
    }

    void WidgetSetDisplayer::updateWidgets(std::span<Widget* const> widgets) {
        ScopeProfiler sp(Profiler::graphic(), "updateWidgets");

        this->widgets.assign(widgets.begin(), widgets.end());

        for (Widget* widget : widgets) {
            std::size_t widgetInstanceId = widget->computeInstanceId();

            WidgetInstanceDisplayer* currentWidgetInstanceDisplayer = findWidgetInstanceDisplayer(*widget);
            if (currentWidgetInstanceDisplayer) {
                if (currentWidgetInstanceDisplayer->getInstanceId() == widgetInstanceId) {
                    continue; //the widget displayer attached to the widget is still valid
                }
                removeWidgetFromDisplayer(*widget, *currentWidgetInstanceDisplayer);
            }

            if (widgetInstanceId == WidgetDisplayable::INSTANCING_DENY_ID) {
                const auto& itFind = widgetDisplayers.find(widget);
                if (itFind != widgetDisplayers.end()) {
                    assert(itFind->second->getInstanceCount() == 0);
                    addWidgetToDisplayer(*widget, *itFind->second);
                    continue; //the widget displayer used in past for this widget has been found
                }
            } else {
                const auto& itFind = widgetInstanceDisplayers.find(widgetInstanceId);
                if (itFind != widgetInstanceDisplayers.end()) {
                    addWidgetToDisplayer(*widget, *itFind->second);
                    continue; //a matching widget instance displayer has been found for the widget
                }
            }

            auto widgetInstanceDisplayer = std::make_unique<WidgetInstanceDisplayer>(*this, uiRenderer);
            addWidgetToDisplayer(*widget, *widgetInstanceDisplayer);
            widgetInstanceDisplayer->initialize();
            if (widgetInstanceId == WidgetDisplayable::INSTANCING_DENY_ID) {
                widgetDisplayers.try_emplace(widget, std::move(widgetInstanceDisplayer));
            } else {
                widgetInstanceDisplayers.try_emplace(widgetInstanceId, std::move(widgetInstanceDisplayer));
            }
        }
    }

    void WidgetSetDisplayer::removeWidget(Widget* widget) {
        if (widget) {
            widgetDisplayers.erase(widget);
            WidgetInstanceDisplayer* widgetInstanceDisplayer = findWidgetInstanceDisplayer(*widget);
            if (widgetInstanceDisplayer) {
                removeWidgetFromDisplayer(*widget, *widgetInstanceDisplayer);
            }

            std::erase(widgets, widget);
        }
    }

    void WidgetSetDisplayer::prepareRendering(unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        ScopeProfiler sp(Profiler::graphic(), "widgetPreRender");

        activeWidgetDisplayers.clear();
        activeWidgetDisplayersOrdered.clear();
        for (const Widget* widget : widgets) {
            WidgetInstanceDisplayer* widgetInstanceDisplayer = findWidgetInstanceDisplayer(*widget);
            if (activeWidgetDisplayers.insert(widgetInstanceDisplayer).second) {
                activeWidgetDisplayersOrdered.push_back(widgetInstanceDisplayer);
                widgetInstanceDisplayer->resetRenderingWidgets();
            }
            widgetInstanceDisplayer->registerRenderingWidget(*widget);
        }
        for (const WidgetInstanceDisplayer* activeWidgetDisplayer : activeWidgetDisplayersOrdered) {
            renderingOrder++;
            activeWidgetDisplayer->prepareRendering(renderingOrder, projectionViewMatrix);
        }
    }

}
