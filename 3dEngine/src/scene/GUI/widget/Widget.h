#ifndef URCHINENGINE_WIDGET_H
#define URCHINENGINE_WIDGET_H

#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "scene/GUI/EventListener.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/target/RenderTarget.h"

namespace urchin {

    class Widget : public Observable {
        public:
            Widget(Position, Size);
            ~Widget() override;

            enum NotificationType {
                SET_IN_FOREGROUND //Widget should be set in the foreground
            };

            enum WidgetStates {
                DEFAULT,
                CLICKING,
                FOCUS
            };

            void onResize(unsigned int, unsigned int);
            virtual void createOrUpdateWidget() = 0;

            virtual void addChild(Widget*);
            virtual void removeChild(Widget*);

            void setParent(Widget*);
            Widget* getParent() const;

            void addEventListener(const std::shared_ptr<EventListener>&);
            const std::vector<std::shared_ptr<EventListener>>& getEventListeners() const;
            WidgetStates getWidgetState() const;

            void setPosition(Position);
            Position getPosition() const;
            int getPositionX() const;
            int getPositionY() const;
            int getGlobalPositionX() const;
            int getGlobalPositionY() const;

            void setSize(Size);
            Size getSize() const;
            unsigned int getWidth() const;
            unsigned int getHeight() const;

            void setIsVisible(bool);
            bool isVisible() const;

            bool onKeyPress(unsigned int);
            virtual bool onKeyPressEvent(unsigned int);
            bool onKeyRelease(unsigned int);
            virtual bool onKeyReleaseEvent(unsigned int);
            bool onChar(unsigned int);
            virtual bool onCharEvent(unsigned int);
            bool onMouseMove(int, int);
            virtual bool onMouseMoveEvent(int, int);
            int getMouseX() const;
            int getMouseY() const;
            virtual void reset();
            void onDisable();

            virtual void display(const RenderTarget*, const ShaderVar&, float);

        protected:
            unsigned int getSceneWidth() const;
            unsigned int getSceneHeight() const;

            const std::vector<Widget*>& getChildren() const;

        private:
            void handleWidgetKeyDown(unsigned int);
            void handleWidgetKeyUp(unsigned int);
            void handleWidgetMouseMove(int, int);
            void handleDisable();

            unsigned int sceneWidth, sceneHeight;

            Widget* parent;
            std::vector<Widget*> children;

            std::vector<std::shared_ptr<EventListener>> eventListeners;
            WidgetStates widgetState;

            Position position;
            Size size;
            bool bIsVisible;

            int mouseX, mouseY;
    };

}

#endif
