#include <scene/ui/widget/checkbox/CheckBox.h>
#include <resources/MediaManager.h>

namespace urchin {

    CheckBox::CheckBox(Position position, Size size, std::string nameSkin) :
            Widget(position, size),
            nameSkin(std::move(nameSkin)),
            bIsChecked(false) {

    }

    std::shared_ptr<CheckBox> CheckBox::newCheckBox(Widget* parent, Position position, Size size, std::string nameSkin) {
        return create<CheckBox>(new CheckBox(position, size, std::move(nameSkin)), parent);
    }

    void CheckBox::createOrUpdateWidget() {
        //skin information
        auto checkBoxChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "checkBox", UdaAttribute("nameSkin", nameSkin));
        texChecked = loadTexture(checkBoxChunk, "imageChecked");
        texUnchecked = loadTexture(checkBoxChunk, "imageUnchecked");

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), 0.0f), Point2<float>((float)getWidth(), (float)getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), (float)getHeight()), Point2<float>(0.0f, (float)getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        checkBoxRenderer = setupUiRenderer("check box", ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(isChecked() ? texChecked : texUnchecked, TextureParam::buildLinear())) //binding 3
                ->enableTransparency()
                ->build();
    }

    std::shared_ptr<Texture> CheckBox::loadTexture(const UdaChunk* checkBoxChunk, const std::string& chunkName) const {
        auto imageElem = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, chunkName, UdaAttribute(), checkBoxChunk);

        auto img = MediaManager::instance()->getMedia<Image>(imageElem->getStringValue());
        return img->createTexture(false);
    }

    void CheckBox::setChecked(bool isChecked) {
        this->bIsChecked = isChecked;
        refreshTexture();
    }

    bool CheckBox::isChecked() const {
        return bIsChecked;
    }

    void CheckBox::refreshTexture() {
        if (checkBoxRenderer) {
            if (bIsChecked) {
                checkBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texChecked, TextureParam::buildLinear()));
            } else {
                checkBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texUnchecked, TextureParam::buildLinear()));
            }
        }
    }

    bool CheckBox::onKeyReleaseEvent(unsigned int) {
        if (getWidgetState() == FOCUS) {
            switchValue();
            return false;
        }
        return true;
    }

    void CheckBox::switchValue() {
        setChecked(!bIsChecked);

        for (auto& eventListener : getEventListeners()) {
            eventListener->onValueChange(this);
        }
    }

    void CheckBox::prepareWidgetRendering(float) {
        updateTranslateVector(checkBoxRenderer.get(), Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        checkBoxRenderer->enableRenderer();
    }
}
