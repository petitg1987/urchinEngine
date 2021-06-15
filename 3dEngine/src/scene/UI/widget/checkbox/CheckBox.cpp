#include <scene/UI/widget/checkbox/CheckBox.h>
#include <resources/MediaManager.h>

namespace urchin {
    CheckBox::CheckBox(Widget* parent, Position position, Size size, std::string nameSkin) :
            Widget(parent, position, size),
            nameSkin(std::move(nameSkin)),
            bIsChecked(false) {

    }

    void CheckBox::createOrUpdateWidget() {
        //skin information
        std::shared_ptr<XmlChunk> checkBoxChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "checkBox", XmlAttribute("nameSkin", nameSkin));
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
                ->addUniformTextureReader(TextureReader::build(texUnchecked, TextureParam::buildNearest())) //binding 3
                ->build();
    }

    std::shared_ptr<Texture> CheckBox::loadTexture(const std::shared_ptr<XmlChunk>& checkBoxChunk, const std::string& chunkName) const {
        std::shared_ptr<XmlChunk> imageEnableElem = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, chunkName, XmlAttribute(), checkBoxChunk);

        auto* img = MediaManager::instance()->getMedia<Image>(imageEnableElem->getStringValue());
        auto tex = img->createTexture(false);
        img->release();
        return tex;
    }

    bool CheckBox::isChecked() const {
        return bIsChecked;
    }

    bool CheckBox::onKeyReleaseEvent(unsigned int) {
        if (getWidgetState() == FOCUS) {
            if (bIsChecked) {
                checkBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texUnchecked, TextureParam::buildNearest()));
            } else {
                checkBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texChecked, TextureParam::buildNearest()));
            }
            bIsChecked = !bIsChecked;
        }
        return true;
    }

    void CheckBox::prepareWidgetRendering(float) {
        updateTranslateVector(checkBoxRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        checkBoxRenderer->enableRenderer();
    }
}
