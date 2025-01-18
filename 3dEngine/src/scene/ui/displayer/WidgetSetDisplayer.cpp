#include <ranges>

#include <scene/ui/displayer/WidgetSetDisplayer.h>
#include <scene/ui/displayer/WidgetInstanceDisplayer.h>

namespace urchin {

    WidgetSetDisplayer::WidgetSetDisplayer(const UIRenderer& uiRenderer) :
            uiRenderer(uiRenderer) {

    }

    WidgetSetDisplayer::~WidgetSetDisplayer() {
        clearDisplayers();
    }

    void WidgetSetDisplayer::onUiRendererSizeUpdated() const {
        for (const std::unique_ptr<WidgetInstanceDisplayer>& displayer : std::views::values(widgetDisplayers)) {
            displayer->onUiRendererSizeUpdated();
        }

        for (const std::unique_ptr<WidgetInstanceDisplayer>& displayer : std::views::values(widgetInstanceDisplayers)) {
            displayer->onUiRendererSizeUpdated();
        }
    }

    void WidgetSetDisplayer::onGammaFactorUpdated() const {
        for (const std::unique_ptr<WidgetInstanceDisplayer>& displayer : std::views::values(widgetDisplayers)) {
            displayer->onGammaFactorUpdated();
        }

        for (const std::unique_ptr<WidgetInstanceDisplayer>& displayer : std::views::values(widgetInstanceDisplayers)) {
            displayer->onGammaFactorUpdated();
        }
    }

    void WidgetSetDisplayer::notify(Observable* observable, int notificationType) {
        if (Widget* widget = dynamic_cast<Widget*>(observable)) {
            WidgetInstanceDisplayer* displayer = findWidgetInstanceDisplayer(*widget);
            if (!displayer) {
                return;
            }

            bool canUpdateDisplayer = false;
            std::size_t newWidgetInstanceId = widget->computeInstanceId();
            if (newWidgetInstanceId != WidgetDisplayable::INSTANCING_DENY_ID && displayer->getInstanceId() != WidgetDisplayable::INSTANCING_DENY_ID) {
                if (displayer->getInstanceCount() <= 1 && !widgetInstanceDisplayers.contains(newWidgetInstanceId)) {
                    //case: displayer is not shared and there isn't other displayer matching the new instance ID
                    canUpdateDisplayer = true;

                    auto displayerNodeHandler = widgetInstanceDisplayers.extract(displayer->getInstanceId());
                    assert(displayerNodeHandler.mapped().get() == displayer);
                    displayerNodeHandler.mapped()->updateInstanceId(newWidgetInstanceId);
                    displayerNodeHandler.key() = newWidgetInstanceId;
                    widgetInstanceDisplayers.insert(std::move(displayerNodeHandler));
                } else if (newWidgetInstanceId == displayer->getInstanceId()) {
                    //case: update scale from 1.0 to 1.0, etc.
                    canUpdateDisplayer = true;
                }
            } else if (newWidgetInstanceId == WidgetDisplayable::INSTANCING_DENY_ID && displayer->getInstanceId() == WidgetDisplayable::INSTANCING_DENY_ID) {
                canUpdateDisplayer = true;
            }

            if (canUpdateDisplayer) {
                if (notificationType == Widget::TEXTURE_UPDATED) {
                    displayer->updateTexture();
                } else if (notificationType == Widget::SIZE_UPDATED) {
                    displayer->updateCoordinates();
                    displayer->updateScissor();
                } else if (notificationType == Widget::POSITION_UPDATED) {
                    displayer->updateScissor();
                } else if (notificationType == Widget::ALPHA_FACTOR_UPDATED) {
                    displayer->updateAlphaFactor();
                }
            } else {
                removeWidgetFromDisplayer(*widget, *displayer);
            }
        }
    }

    WidgetInstanceDisplayer* WidgetSetDisplayer::findWidgetInstanceDisplayer(const Widget& widget) const {
        #ifdef URCHIN_DEBUG
            assert(widget.getWidgetInstanceDisplayers().size() <= 1); //currently only one WidgetSetDisplayer exists for a widget, and it is stored in UIRenderer
        #endif
        for (WidgetInstanceDisplayer* widgetInstanceDisplayer : widget.getWidgetInstanceDisplayers()) {
            if (&widgetInstanceDisplayer->getWidgetSetDisplayer() == this) {
                return widgetInstanceDisplayer;
            }
        }
        return nullptr;
    }

    void WidgetSetDisplayer::clearDisplayers() {
        for (Widget* widget : std::views::keys(widgetDisplayers)) {
            unobserveWidgetUpdate(*widget);
        }
        widgetDisplayers.clear();

        for (const std::unique_ptr<WidgetInstanceDisplayer>& displayer : std::views::values(widgetInstanceDisplayers)) {
            for (Widget* widget : displayer->getInstanceWidgets()) {
                unobserveWidgetUpdate(*widget);
            }
        }
        widgetInstanceDisplayers.clear();
    }

    void WidgetSetDisplayer::removeWidgetFromDisplayer(Widget& widget, WidgetInstanceDisplayer& widgetInstanceDisplayer) {
        widgetInstanceDisplayer.removeInstanceWidget(widget);
        unobserveWidgetUpdate(widget);

        if (widgetInstanceDisplayer.getInstanceCount() == 0) {
            //to do:
            // - remove the displayer based on strategy to define (unused for x seconds, too much unused displayers, etc.)
            // - erase displayer from widgetDisplayers/widgetInstanceDisplayers
        }
    }

    void WidgetSetDisplayer::addWidgetToDisplayer(Widget& widget, WidgetInstanceDisplayer& widgetInstanceDisplayer) {
        widgetInstanceDisplayer.addInstanceWidget(widget);
        observeWidgetUpdate(widget);
    }

    void WidgetSetDisplayer::observeWidgetUpdate(Widget& widget) {
        widget.addObserver(this, Widget::TEXTURE_UPDATED);
        widget.addObserver(this, Widget::SIZE_UPDATED);
        widget.addObserver(this, Widget::POSITION_UPDATED);
        widget.addObserver(this, Widget::ALPHA_FACTOR_UPDATED);
    }

    void WidgetSetDisplayer::unobserveWidgetUpdate(Widget& widget) {
        widget.removeObserver(this, Widget::ALPHA_FACTOR_UPDATED);
        widget.removeObserver(this, Widget::POSITION_UPDATED);
        widget.removeObserver(this, Widget::SIZE_UPDATED);
        widget.removeObserver(this, Widget::TEXTURE_UPDATED);
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

    std::span<Widget* const> WidgetSetDisplayer::getWidgets() const {
        return widgets;
    }

    void WidgetSetDisplayer::prepareRendering(unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        ScopeProfiler sp(Profiler::graphic(), "widgetPreRender");

        activeWidgetDisplayers.clear();
        for (const Widget* widget : widgets) {
            WidgetInstanceDisplayer* widgetInstanceDisplayer = findWidgetInstanceDisplayer(*widget);
            bool unknownInstanceDisplayer = std::ranges::none_of(activeWidgetDisplayers, [widgetInstanceDisplayer](const WidgetInstanceDisplayer* v) {return v == widgetInstanceDisplayer;});
            if (unknownInstanceDisplayer) {
                activeWidgetDisplayers.push_back(widgetInstanceDisplayer);
                widgetInstanceDisplayer->resetRenderingWidgets();
            }
            widgetInstanceDisplayer->registerRenderingWidget(*widget);
        }
        for (const WidgetInstanceDisplayer* activeWidgetDisplayer : activeWidgetDisplayers) {
            renderingOrder++;
            activeWidgetDisplayer->prepareRendering(renderingOrder, projectionViewMatrix);
        }
    }

}
