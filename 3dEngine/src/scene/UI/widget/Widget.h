#pragma once

#include <vector>
#include <memory>
#include <UrchinCommon.h>

#include <scene/UI/widget/Position.h>
#include <scene/UI/widget/Size.h>
#include <scene/UI/UISkinService.h>
#include <scene/UI/EventListener.h>
#include <scene/UI/i18n/I18nService.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    class Widget : public Observable {
        public:
            Widget(Widget*, Position, Size);
            ~Widget() override;

            enum NotificationType {
                SET_IN_FOREGROUND //Widget should be set in the foreground
            };

            enum WidgetStates {
                DEFAULT,
                CLICKING,
                FOCUS
            };

            void initialize(std::shared_ptr<RenderTarget>, std::shared_ptr<Shader>, I18nService*);
            void onResize(unsigned int, unsigned int, const Matrix4<float>&);

            Widget* getParent() const;
            const std::vector<Widget*>& getChildren() const;
            void deleteChildren();

            void addEventListener(const std::shared_ptr<EventListener>&);
            const std::vector<std::shared_ptr<EventListener>>& getEventListeners() const;
            WidgetStates getWidgetState() const;

            void setPosition(Position);
            Position getPosition() const;
            int getPositionX() const;
            int getPositionY() const;
            const WidgetOutline& getOutline() const;
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

            void prepareRendering(float);

        protected:
            std::shared_ptr<GenericRendererBuilder> setupUiRenderer(const std::string&, ShapeType) const;
            void updateTranslateVector(const std::shared_ptr<GenericRenderer>&, const Vector2<int>&) const;
            const std::shared_ptr<RenderTarget>& getRenderTarget() const;
            unsigned int getSceneWidth() const;
            unsigned int getSceneHeight() const;
            virtual void createOrUpdateWidget() = 0;

            I18nService* getI18nService() const;

            virtual void prepareWidgetRendering(float) = 0;

            WidgetOutline widgetOutline;

        private:
            void handleWidgetKeyDown(unsigned int);
            void handleWidgetKeyUp(unsigned int);
            void handleWidgetMouseMove(int, int);
            void handleDisable();

            std::shared_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Shader> shader;
            unsigned int sceneWidth, sceneHeight;
            Matrix4<float> projectionMatrix;

            Widget* parent;
            std::vector<Widget*> children;

            std::vector<std::shared_ptr<EventListener>> eventListeners;
            I18nService* i18nService;
            WidgetStates widgetState;

            Position position;
            Size size;
            bool bIsVisible;

            int mouseX, mouseY;
    };

}
