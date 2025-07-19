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
        for (const std::unique_ptr<WidgetInstanceDisplayer>& displayer : std::views::values(exclusiveInstanceDisplayers)) {
            displayer->onUiRendererSizeUpdated();
        }

        for (const std::unique_ptr<WidgetInstanceDisplayer>& displayer : std::views::values(shareableInstanceDisplayers)) {
            displayer->onUiRendererSizeUpdated();
        }
    }

    void WidgetSetDisplayer::onGammaFactorUpdated() const {
        for (const std::unique_ptr<WidgetInstanceDisplayer>& displayer : std::views::values(exclusiveInstanceDisplayers)) {
            displayer->onGammaFactorUpdated();
        }

        for (const std::unique_ptr<WidgetInstanceDisplayer>& displayer : std::views::values(shareableInstanceDisplayers)) {
            displayer->onGammaFactorUpdated();
        }
    }

    void WidgetSetDisplayer::notify(Observable* observable, int notificationType) {
        if (auto widget = dynamic_cast<Widget*>(observable)) {
            WidgetInstanceDisplayer* displayer = findWidgetInstanceDisplayer(*widget);
            if (!displayer) {
                return;
            }

            bool canUpdateDisplayer = false;
            std::size_t newWidgetInstanceId = widget->computeInstanceId();
            if (newWidgetInstanceId != WidgetDisplayable::INSTANCING_DENY_ID && displayer->getInstanceId() != WidgetDisplayable::INSTANCING_DENY_ID) {
                if (displayer->getInstanceCount() <= 1 && !shareableInstanceDisplayers.contains(newWidgetInstanceId)) {
                    //case: displayer is not shared and there isn't other displayer matching the new instance ID
                    canUpdateDisplayer = true;

                    auto displayerNodeHandler = shareableInstanceDisplayers.extract(displayer->getInstanceId());
                    assert(displayerNodeHandler.mapped().get() == displayer);
                    displayerNodeHandler.mapped()->updateInstanceId(newWidgetInstanceId);
                    displayerNodeHandler.key() = newWidgetInstanceId;
                    shareableInstanceDisplayers.insert(std::move(displayerNodeHandler));
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
                //The displayer will be replaced in the 'updateWidgets' method, but it is already detached here.
                //Indeed, the updated value in the widget could be wrongly used by another update of widget in the displayer via WidgetInstanceDisplayer#getReferenceWidget().
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
        for (Widget* widget : std::views::keys(exclusiveInstanceDisplayers)) {
            unobserveWidgetUpdate(*widget);
        }
        exclusiveInstanceDisplayers.clear();

        for (const std::unique_ptr<WidgetInstanceDisplayer>& displayer : std::views::values(shareableInstanceDisplayers)) {
            for (Widget* widget : displayer->getInstanceWidgets()) {
                unobserveWidgetUpdate(*widget);
            }
        }
        shareableInstanceDisplayers.clear();
    }

    void WidgetSetDisplayer::removeWidgetFromDisplayer(Widget& widget, WidgetInstanceDisplayer& widgetInstanceDisplayer) {
        widgetInstanceDisplayer.removeInstanceWidget(widget);
        unobserveWidgetUpdate(widget);
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
                const auto& itFind = exclusiveInstanceDisplayers.find(widget);
                if (itFind != exclusiveInstanceDisplayers.end()) {
                    assert(itFind->second->getInstanceCount() == 0);
                    addWidgetToDisplayer(*widget, *itFind->second);
                    continue; //the widget displayer used in past for this widget has been found
                }
            } else {
                const auto& itFind = shareableInstanceDisplayers.find(widgetInstanceId);
                if (itFind != shareableInstanceDisplayers.end()) {
                    addWidgetToDisplayer(*widget, *itFind->second);
                    continue; //a matching widget instance displayer has been found for the widget
                }
            }

            auto widgetInstanceDisplayer = std::make_unique<WidgetInstanceDisplayer>(*this, uiRenderer);
            addWidgetToDisplayer(*widget, *widgetInstanceDisplayer);
            widgetInstanceDisplayer->initialize();
            if (widgetInstanceId == WidgetDisplayable::INSTANCING_DENY_ID) {
                exclusiveInstanceDisplayers.try_emplace(widget, std::move(widgetInstanceDisplayer));
            } else {
                shareableInstanceDisplayers.try_emplace(widgetInstanceId, std::move(widgetInstanceDisplayer));
            }
        }
    }

    void WidgetSetDisplayer::removeWidget(Widget* widget) {
        if (widget) {
            exclusiveInstanceDisplayers.erase(widget);
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

    void WidgetSetDisplayer::prepareRendering(unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix, const Vector2<float>& cameraJitter) {
        ScopeProfiler sp(Profiler::graphic(), "widgetPreRender");

        activeInstanceDisplayers.clear();
        activeSortedInstanceDisplayers.clear();
        for (const Widget* widget : widgets) {
            WidgetInstanceDisplayer* widgetInstanceDisplayer = findWidgetInstanceDisplayer(*widget);
            if (activeInstanceDisplayers.insert(widgetInstanceDisplayer)) {
                activeSortedInstanceDisplayers.push_back(widgetInstanceDisplayer);
                widgetInstanceDisplayer->resetRenderingWidgets();
            }
            widgetInstanceDisplayer->registerRenderingWidget(*widget);
        }
        for (WidgetInstanceDisplayer* activeWidgetDisplayer : activeSortedInstanceDisplayers) {
            renderingOrder++;
            activeWidgetDisplayer->prepareRendering(renderingOrder, projectionViewMatrix, cameraJitter);
        }

        purgeUnusedInstanceDisplayers();
    }

    void WidgetSetDisplayer::purgeUnusedInstanceDisplayers() {
        constexpr double OLD_DISPLAYERS_TIME_MS = 10'000.0;
        auto purgeOldInstanceDisplayers = [](auto& instanceDisplayersMap) {
            auto currentTime = std::chrono::steady_clock::now();
            for (auto it = instanceDisplayersMap.begin(); it != instanceDisplayersMap.end();) {
                auto unusedTimeInMs = (double)std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - it->second->getLastRenderingTime()).count();
                if (unusedTimeInMs >= OLD_DISPLAYERS_TIME_MS) {
                    it = instanceDisplayersMap.erase(it);
                } else {
                    ++it;
                }
            }
        };
        purgeOldInstanceDisplayers(exclusiveInstanceDisplayers);
        purgeOldInstanceDisplayers(shareableInstanceDisplayers);
    }

}
