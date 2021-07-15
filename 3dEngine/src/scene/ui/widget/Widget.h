#pragma once

#include <vector>
#include <memory>
#include <UrchinCommon.h>

#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/UISkinService.h>
#include <scene/ui/EventListener.h>
#include <i18n/I18nService.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/GenericRendererBuilder.h>

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

            void initialize(RenderTarget&, const Shader&, I18nService&);
            void onResize(unsigned int, unsigned int);

            Widget* getParent() const;

            void addChild(const std::shared_ptr<Widget>&);
            const std::vector<std::shared_ptr<Widget>>& getChildren() const;
            void detachChild(Widget*);
            void detachChildren();

            void addEventListener(std::shared_ptr<EventListener>);
            const std::vector<std::shared_ptr<EventListener>>& getEventListeners() const;
            WidgetStates getWidgetState() const;

            void updatePosition(Position);
            Position getPosition() const;
            int getPositionX() const;
            int getPositionY() const;
            const WidgetOutline& getOutline() const;
            int getGlobalPositionX() const;
            int getGlobalPositionY() const;

            void updateSize(Size);
            Size getSize() const;
            unsigned int getWidth() const;
            unsigned int getHeight() const;

            void setIsVisible(bool);
            bool isVisible() const;

            bool onKeyPress(unsigned int);
            bool onKeyRelease(unsigned int);
            bool onChar(char32_t);
            bool onMouseMove(int, int);
            int getMouseX() const;
            int getMouseY() const;
            virtual void onResetState();

            void prepareRendering(float);

        protected:
            template<class T> static std::shared_ptr<T> create(T*, Widget*);

            std::shared_ptr<GenericRendererBuilder> setupUiRenderer(const std::string&, ShapeType) const;
            void updateTranslateVector(GenericRenderer*, const Vector2<int>&) const;
            RenderTarget& getRenderTarget() const;
            unsigned int getSceneWidth() const;
            unsigned int getSceneHeight() const;

            virtual void createOrUpdateWidget() = 0;
            void setSize(Size);

            virtual bool onKeyPressEvent(unsigned int);
            virtual bool onKeyReleaseEvent(unsigned int);
            virtual bool onCharEvent(char32_t);
            virtual bool onMouseMoveEvent(int, int);

            virtual void prepareWidgetRendering(float) = 0;

            WidgetOutline widgetOutline;
            I18nService* i18nService;

        private:
            bool handleWidgetKeyDown(unsigned int);
            bool handleWidgetKeyUp(unsigned int);
            bool handleWidgetMouseMove(int, int);
            void handleWidgetReset();

            RenderTarget* renderTarget;
            const Shader* shader;
            unsigned int sceneWidth, sceneHeight;

            Widget* parent;
            std::vector<std::shared_ptr<Widget>> children;

            std::vector<std::shared_ptr<EventListener>> eventListeners;
            WidgetStates widgetState;

            Position position;
            Size size;
            bool bIsVisible;

            int mouseX, mouseY;
    };

    #include "Widget.inl"

}
