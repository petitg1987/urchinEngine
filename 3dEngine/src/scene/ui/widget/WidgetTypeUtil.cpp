#include <scene/ui/widget/WidgetTypeUtil.h>

namespace urchin {

    std::string WidgetTypeUtil::toWidgetTypeString(WidgetType widgetType) {
        if (widgetType == WidgetType::UNKNOWN) {
            return "unknown";
        } else if (widgetType == WidgetType::BUTTON) {
            return "button";
        } else if (widgetType == WidgetType::CHECKBOX) {
            return "checkbox";
        } else if (widgetType == WidgetType::CONTAINER) {
            return "container";
        } else if (widgetType == WidgetType::LAZY_WIDGET) {
            return "lazyWidget";
        } else if (widgetType == WidgetType::SEQUENCE) {
            return "sequence";
        } else if (widgetType == WidgetType::SLIDER) {
            return "slider";
        } else if (widgetType == WidgetType::STATIC_BITMAP) {
            return "staticBitmap";
        } else if (widgetType == WidgetType::TEXT) {
            return "text";
        } else if (widgetType == WidgetType::TEXTBOX) {
            return "textbox";
        } else if (widgetType == WidgetType::TEXTAREA) {
            return "textarea";
        } else if (widgetType == WidgetType::WINDOW) {
            return "window";
        }
        throw std::runtime_error("Unknown widget type: " + std::to_string((int)widgetType));
    }
}
