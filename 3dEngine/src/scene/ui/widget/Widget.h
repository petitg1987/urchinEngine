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
#include <scene/ui/ClipboardLocal.h>
#include <scene/ui/displayer/WidgetDisplayable.h>
#include <i18n/I18nService.h>
#include <graphics/api/GraphicsApi.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    class UIRenderer;
    struct UI3dData;
    class Container;

    class Widget : public Observable, public WidgetDisplayable {
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
            virtual void uninitialize();
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

            void applyUpdatedGammaFactor();
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
            bool isScissorEnabled() const;

            void prepareRendering(float, unsigned int&, const Matrix4<float>&);

        protected:
            template<class T> static std::shared_ptr<T> create(T*, Widget*);

            I18nService& getI18nService() const;
            UI3dData* getUi3dData() const;
            Clipboard& getClipboard() const;

            virtual void createOrUpdateWidget() = 0;
            std::shared_ptr<GenericRendererBuilder> baseRendererBuilder(std::string, ShapeType, bool);
            virtual void refreshCoordinates();
            std::vector<Point2<float>>& getVertexCoordinates();
            std::vector<Point2<float>>& getTextureCoordinates();
            void setupRenderer(std::unique_ptr<GenericRenderer>);
            GenericRenderer* getRenderer() const;

            WidgetOutline& getOutline();
            const WidgetOutline& getOutline() const;
            TextureParam::Anisotropy getTextureAnisotropy() const;

            virtual bool onKeyPressEvent(InputDeviceKey);
            virtual bool onKeyReleaseEvent(InputDeviceKey);
            virtual bool onCharEvent(char32_t);
            virtual bool onMouseMoveEvent(int, int);
            virtual bool onScrollEvent(double);
            virtual void onResetStateEvent();

            virtual void prepareWidgetRendering(float);

        private:
            bool handleWidgetKeyPress(InputDeviceKey);
            bool handleWidgetKeyRelease(InputDeviceKey);
            bool handleWidgetMouseMove(int, int);
            void handleWidgetResetState();
            bool isMouseOnWidget(int, int) const;

            unsigned int computeDepthLevel() const;
            void refreshScissor(bool);

            UIRenderer* uiRenderer;
            int mouseX;
            int mouseY;
            Widget* parent;
            std::vector<std::shared_ptr<Widget>> children;
            std::vector<std::shared_ptr<EventListener>> eventListeners;

            std::vector<Point2<float>> vertexCoord;
            std::vector<Point2<float>> textureCoord;
            std::unique_ptr<GenericRenderer> renderer;

            WidgetOutline widgetOutline;
            WidgetState widgetState;

            Position position;
            Size size;
            Vector2<float> scale;
            float rotationZ;
            struct ColorParams {
                alignas(4) float alphaFactor;
                alignas(4) float gammaFactor;
            };
            ColorParams colorParams;
            bool scissorEnabled;
            Vector2<int> scissorOffset;
            Vector2<int> scissorSize;
            bool bIsVisible;
    };

    #include "Widget.inl"

}
