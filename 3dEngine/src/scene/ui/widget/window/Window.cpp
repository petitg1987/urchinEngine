#include <memory>
#include <utility>
#include <UrchinCommon.h>

#include <scene/ui/widget/window/Window.h>
#include <scene/InputDeviceKey.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    Window::Window(Widget* parent, Position position, Size size, std::string nameSkin, std::string titleKey) :
            Widget(parent, position, size),
            nameSkin(std::move(nameSkin)),
            titleKey(std::move(titleKey)),
            mousePositionX(0),
            mousePositionY(0),
            state(DEFAULT),
            title(nullptr) {
        if(parent) {
            Window::createOrUpdateWidget();
        }
    }

    void Window::createOrUpdateWidget() {
        //skin information
        auto windowChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "window", XmlAttribute("nameSkin", nameSkin));

        auto skinChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "skin", XmlAttribute(), windowChunk.get());
        texWindow = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinChunk.get(), &widgetOutline);

        if (!titleKey.empty()) {
            auto textSkinChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "textSkin", XmlAttribute(), windowChunk.get());
            delete title;
            title = Text::newTranslatableText(this, Position(0, 0, LengthType::PIXEL), textSkinChunk->getStringValue(), titleKey);
            title->updatePosition(Position(0.0f, -((float)widgetOutline.topWidth + (float)title->getHeight()) / 2.0f, LengthType::PIXEL));
        }

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), 0.0f), Point2<float>((float)getWidth(), (float)getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), (float)getHeight()), Point2<float>(0.0f, (float)getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        windowRenderer = setupUiRenderer("window", ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(texWindow, TextureParam::buildLinear())) //binding 2
                ->build();
    }

    bool Window::onKeyPressEvent(unsigned int key) {
        Rectangle<int> titleZone(Point2<int>(getGlobalPositionX(), getGlobalPositionY()),
                Point2<int>(getGlobalPositionX() + ((int)getWidth() - widgetOutline.rightWidth), getGlobalPositionY() + widgetOutline.topWidth));
        Rectangle<int> closeZone(Point2<int>(getGlobalPositionX()+((int)getWidth() - widgetOutline.rightWidth), getGlobalPositionY()),
                Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + widgetOutline.topWidth));

        if (key == InputDeviceKey::MOUSE_LEFT && titleZone.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
            mousePositionX = getMouseX() - getPositionX();
            mousePositionY = getMouseY() - getPositionY();

            state = MOVING;
        } else if (key == InputDeviceKey::MOUSE_LEFT && closeZone.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
            state = CLOSING;
        }

        Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()),
                Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));
        bool propagateEvent = true;
        if (key == InputDeviceKey::MOUSE_LEFT && widgetRectangle.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
            notifyObservers(this, SET_IN_FOREGROUND);
            propagateEvent = false;
        }

        return propagateEvent;
    }

    bool Window::onKeyReleaseEvent(unsigned int key) {
        Rectangle<int> closeZone(Point2<int>(getGlobalPositionX() + ((int)getWidth() - widgetOutline.rightWidth), getGlobalPositionY()),
                Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + widgetOutline.topWidth));
        if (key == InputDeviceKey::MOUSE_LEFT && state == CLOSING && closeZone.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
            setIsVisible(false);
        }

        state = DEFAULT;

        return true;
    }

    bool Window::onMouseMoveEvent(int mouseX, int mouseY) {
        if (state == MOVING) {
            auto positionX = (float)(mouseX - mousePositionX);
            if (getPosition().getPositionTypeX() == LengthType::PERCENTAGE) {
                positionX = (positionX / (float)getSceneWidth()) * 100.0f;
            }

            auto positionY = (float)(mouseY - mousePositionY);
            if (getPosition().getPositionTypeY() == LengthType::PERCENTAGE) {
                positionY = (positionY / (float)getSceneHeight()) * 100.0f;
            }

            updatePosition(Position(positionX, getPosition().getPositionTypeX(), positionY, getPosition().getPositionTypeY()));
        }

        return true;
    }

    void Window::prepareWidgetRendering(float) {
        updateTranslateVector(windowRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        windowRenderer->enableRenderer();
    }

}
