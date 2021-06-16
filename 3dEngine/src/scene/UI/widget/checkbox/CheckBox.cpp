#include <scene/UI/widget/checkbox/CheckBox.h>
#include <resources/MediaManager.h>

namespace urchin {
    CheckBox::CheckBox(Widget* parent, Position position, Size size, const std::string& nameSkin) :
            Widget(parent, position, size),
            bIsChecked(false) {
        std::shared_ptr<XmlChunk> checkBoxChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "checkBox", XmlAttribute("nameSkin", nameSkin));
        texChecked = loadTexture(checkBoxChunk, "imageChecked");
        texUnchecked = loadTexture(checkBoxChunk, "imageUnchecked");
    }

    void CheckBox::createOrUpdateWidget() {
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
                ->build();
    }

    std::shared_ptr<Texture> CheckBox::loadTexture(const std::shared_ptr<XmlChunk>& checkBoxChunk, const std::string& chunkName) const {
        std::shared_ptr<XmlChunk> imageEnableElem = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, chunkName, XmlAttribute(), checkBoxChunk);

        auto* img = MediaManager::instance()->getMedia<Image>(imageEnableElem->getStringValue());
        auto tex = img->createTexture(false);
        img->release();
        return tex;
    }

    void CheckBox::setChecked(bool isChecked) {
        this->bIsChecked = isChecked;
        refreshTexture();
    }

    bool CheckBox::isChecked() const {
        return bIsChecked;
    }

    void CheckBox::refreshTexture() {
        if(checkBoxRenderer) {
            if (bIsChecked) {
                checkBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texUnchecked, TextureParam::buildLinear()));
            } else {
                checkBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texChecked, TextureParam::buildLinear()));
            }
        }
    }

    bool CheckBox::onKeyReleaseEvent(unsigned int) {
        if (getWidgetState() == FOCUS) {
            refreshTexture();
            bIsChecked = !bIsChecked;
            return false;
        }
        return true;
    }

    void CheckBox::prepareWidgetRendering(float) {
        updateTranslateVector(checkBoxRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        checkBoxRenderer->enableRenderer();
    }
}
