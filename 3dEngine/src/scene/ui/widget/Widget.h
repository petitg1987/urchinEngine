#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <UrchinCommon.h>

#include <scene/InputDeviceKey.h>
#include <scene/ui/widget/WidgetType.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/UISkinService.h>
#include <scene/ui/EventListener.h>
#include <scene/ui/Clipboard.h>
#include <i18n/I18nService.h>
#include <graphics/api/GraphicsApi.h>
#include <graphics/render/GenericRendererBuilder.h>

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

            enum WidgetState {
                DEFAULT,
                CLICKING,
                FOCUS
            };

            void initialize(UIRenderer&);
            bool isInitialized() const;
            virtual void onResize();
            void onCameraProjectionUpdate();

            virtual WidgetType getWidgetType() const = 0;

            Widget* getParent() const;
            Container* getParentContainer() const;

            virtual void addChild(const std::shared_ptr<Widget>&);
            const std::vector<std::shared_ptr<Widget>>& getChildren() const;
            virtual void detachChild(Widget*);
            virtual void detachChildren();

            void clearEventListeners();
            void addEventListener(std::shared_ptr<EventListener>);
            const std::vector<std::shared_ptr<EventListener>>& getEventListeners() const;
            WidgetState getWidgetState() const;
            Point2<int> getSceneSize() const;

            void updatePosition(Position);
            Position getPosition() const;
            float getPositionX() const;
            float getPositionY() const;
            const WidgetOutline& getOutline() const;
            float getGlobalPositionX() const;
            float getGlobalPositionY() const;

            virtual void updateSize(Size);
            Size getSize() const;
            float getWidth() const;
            float getHeight() const;
            Rectangle2D<int> widgetRectangle() const;

            void updateScale(const Vector2<float>&);
            const Vector2<float>& getScale() const;

            void updateRotation(float);
            float getRotation() const;

            void updateAlphaFactor(float);
            float getAlphaFactor() const;

            template<class T> float widthLengthToPixel(float, LengthType, const T&) const;
            float widthPixelToLength(float, LengthType) const;
            template<class T> float heightLengthToPixel(float, LengthType, const T&) const;
            float heightPixelToLength(float, LengthType) const;

            void setIsVisible(bool);
            bool isVisible() const;

            bool onKeyPress(InputDeviceKey);
            bool onKeyRelease(InputDeviceKey);
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
            void updateProperties(GenericRenderer*, const Matrix4<float>&, const Vector2<float>&) const;

            I18nService& getI18nService() const;
            UI3dData* getUi3dData() const;
            Clipboard& getClipboard() const;

            virtual void createOrUpdateWidget() = 0;
            void setSize(Size);

            virtual bool onKeyPressEvent(InputDeviceKey);
            virtual bool onKeyReleaseEvent(InputDeviceKey);
            virtual bool onCharEvent(char32_t);
            virtual bool onMouseMoveEvent(int, int);
            virtual bool onScrollEvent(double);
            virtual void onResetStateEvent();

            virtual void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&);

            WidgetOutline widgetOutline;

        private:
            bool handleWidgetKeyPress(InputDeviceKey);
            bool handleWidgetKeyRelease(InputDeviceKey);
            bool handleWidgetMouseMove(int, int);
            void handleWidgetResetState();
            bool isMouseOnWidget(int, int) const;

            unsigned int computeDepthLevel() const;
            void refreshScissor(bool);

            UIRenderer* uiRenderer;

            Widget* parent;
            std::vector<std::shared_ptr<Widget>> children;

            std::vector<std::shared_ptr<EventListener>> eventListeners;
            WidgetState widgetState;

            Position position;
            Size size;
            Vector2<float> scale;
            float rotationZ;
            float alphaFactor;
            bool scissorEnabled;
            Vector2<int> scissorOffset;
            Vector2<int> scissorSize;
            bool bIsVisible;

            int mouseX;
            int mouseY;
    };

    #include "Widget.inl"

}
