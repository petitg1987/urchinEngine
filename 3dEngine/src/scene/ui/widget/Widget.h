#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <UrchinCommon.h>

#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/UISkinService.h>
#include <scene/ui/EventListener.h>
#include <i18n/I18nService.h>
#include <api/render/target/RenderTarget.h>
#include <api/render/GenericRendererBuilder.h>

namespace urchin {

    class UIRenderer;
    struct UI3dData;
    class Container;

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

            void initialize(UIRenderer&);
            bool isInitialized() const;
            virtual void onResize();
            void onCameraProjectionUpdate();

            Widget* getParent() const;
            Container* getParentContainer() const;

            virtual void addChild(const std::shared_ptr<Widget>&);
            const std::vector<std::shared_ptr<Widget>>& getChildren() const;
            virtual void detachChild(Widget*);
            virtual void detachChildren();

            void clearEventListeners();
            void addEventListener(std::shared_ptr<EventListener>);
            const std::vector<std::shared_ptr<EventListener>>& getEventListeners() const;
            WidgetStates getWidgetState() const;
            Point2<unsigned int> getSceneSize() const;

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
            Rectangle2D<int> widgetRectangle() const;

            void updateScale(const Vector2<float>&);
            const Vector2<float>& getScale() const;

            template<class T> int widthLengthToPixel(float, LengthType, const T&) const;
            float widthPixelToLength(float, LengthType) const;
            template<class T> int heightLengthToPixel(float, LengthType, const T&) const;
            float heightPixelToLength(float, LengthType) const;

            void setIsVisible(bool);
            bool isVisible() const;

            bool onKeyPress(unsigned int);
            bool onKeyRelease(unsigned int);
            bool onChar(char32_t);
            bool onMouseMove(int, int);
            bool onScroll(double);
            void onResetState();
            int getMouseX() const;
            int getMouseY() const;

            void prepareRendering(float, unsigned int&, const Matrix4<float>&);

        protected:
            template<class T> static std::shared_ptr<T> create(T*, Widget*);

            std::shared_ptr<GenericRendererBuilder> setupUiRenderer(std::string, ShapeType, bool) const;
            TextureParam::Anisotropy getTextureAnisotropy() const;
            void updatePositioning(GenericRenderer*, const Matrix4<float>&, const Vector2<int>&) const;

            I18nService* getI18nService() const;
            UI3dData* getUi3dData() const;

            virtual void createOrUpdateWidget() = 0;
            void setSize(Size);

            virtual bool onKeyPressEvent(unsigned int);
            virtual bool onKeyReleaseEvent(unsigned int);
            virtual bool onCharEvent(char32_t);
            virtual bool onMouseMoveEvent(int, int);
            virtual bool onScrollEvent(double);
            virtual void onResetStateEvent();

            virtual void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&);

            WidgetOutline widgetOutline;

        private:
            bool handleWidgetKeyPress(unsigned int);
            bool handleWidgetKeyRelease(unsigned int);
            bool handleWidgetMouseMove(int, int);
            void handleWidgetResetState();
            bool isMouseOnWidget(int, int) const;

            unsigned int computeDepthLevel() const;

            UIRenderer* uiRenderer;

            Widget* parent;
            std::vector<std::shared_ptr<Widget>> children;

            std::vector<std::shared_ptr<EventListener>> eventListeners;
            WidgetStates widgetState;

            Position position;
            Size size;
            Vector2<float> scale;
            bool bIsVisible;

            int mouseX, mouseY;
    };

    #include "Widget.inl"

}
