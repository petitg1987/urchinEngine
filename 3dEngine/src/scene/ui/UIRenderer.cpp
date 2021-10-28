#include <algorithm>
#include <utility>

#include <scene/ui/UIRenderer.h>
#include <resources/ResourceRetriever.h>
#include <resources/font/Font.h>
#include <api/render/shader/builder/ShaderBuilder.h>

namespace urchin {

    //debug parameters
    bool DEBUG_DISPLAY_FONT_TEXTURE = false;

    UIRenderer::UIRenderer(RenderTarget& renderTarget, I18nService& i18nService) :
            renderTarget(renderTarget),
            i18nService(i18nService),
            sceneSize(renderTarget.getWidth(), renderTarget.getHeight()) {
        if (renderTarget.isValidRenderTarget()) {
            uiShader = ShaderBuilder::createShader("ui.vert.spv", "", "ui.frag.spv");
        } else {
            uiShader = ShaderBuilder::createNullShader();
        }
    }

    void UIRenderer::initializeFor3dUi(const Matrix4<float>& cameraProjectionMatrix, const Transform<float>& transform) {
        assert(this->widgets.empty());
        assert(!this->cameraProjectionMatrix.has_value());

        this->cameraProjectionMatrix = cameraProjectionMatrix;
        this->transform = transform;

        if (renderTarget.isValidRenderTarget()) {
            uiShader = ShaderBuilder::createShader("ui3d.vert.spv", "", "ui3d.frag.spv");
        } else {
            uiShader = ShaderBuilder::createNullShader();
        }
    }

    void UIRenderer::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneSize = Point2<unsigned int>(sceneWidth, sceneHeight);

        //widgets resize
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            widgets[(std::size_t)i]->onResize(sceneWidth, sceneHeight);
        }

        //debug
        if (DEBUG_DISPLAY_FONT_TEXTURE) {
            auto font = ResourceRetriever::instance().getResource<Font>("UI/fontText.ttf", {{"fontSize", "16"}, {"fontColor", "1.0 1.0 1.0"}});

            auto textureDisplayer = std::make_unique<TextureRenderer>(font->getTexture(), TextureRenderer::DEFAULT_VALUE);
            textureDisplayer->setPosition(TextureRenderer::USER_DEFINED_X, TextureRenderer::USER_DEFINED_Y);
            textureDisplayer->setSize(20.0f, (float)font->getDimensionTexture() + 20.0f, 20.0f, (float)font->getDimensionTexture() + 20.0f);
            textureDisplayer->enableTransparency();
            textureDisplayer->initialize("[DEBUG] font texture", renderTarget, sceneWidth, sceneHeight, -1.0f, -1.0f);
            debugFont = std::move(textureDisplayer);
        }
    }

    void UIRenderer::onCameraProjectionUpdate(const Matrix4<float>& projectionMatrix) {
        if (!cameraProjectionMatrix.has_value()) {
            throw std::runtime_error("UI renderer has not been initialized for 3d UI");
        }
        this->cameraProjectionMatrix = projectionMatrix;

        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            widgets[(std::size_t)i]->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void UIRenderer::notify(Observable* observable, int notificationType) {
        if (auto* widget = dynamic_cast<Widget*>(observable)) {
            if (notificationType == Widget::SET_IN_FOREGROUND) {
                auto itFind = std::find_if(widgets.begin(), widgets.end(), [&widget](const auto& o){return widget == o.get();});
                std::shared_ptr<Widget> widgetPtr = *itFind;
                widgets.erase(itFind);
                widgets.push_back(widgetPtr);

                //reset the others widgets
                for (long i = (long)widgets.size() - 2; i >= 0; --i) {
                    widgets[(std::size_t)i]->onResetState();
                }
            }
        }
    }

    bool UIRenderer::onKeyPress(unsigned int key) {
        //keep a temporary copy of the widgets in case the underlying action goal is to destroy the widgets
        std::vector<std::shared_ptr<Widget>> widgetsCopy = widgets;
        for (long i = (long)widgetsCopy.size() - 1; i >= 0; --i) {
            if (!widgetsCopy[(std::size_t)i]->onKeyPress(key)) {
                return false;
            }
        }
        return true;
    }

    bool UIRenderer::onKeyRelease(unsigned int key) {
        //keep a temporary copy of the widgets in case the underlying action goal is to destroy the widgets
        std::vector<std::shared_ptr<Widget>> widgetsCopy = widgets;
        for (long i = (long)widgetsCopy.size() - 1; i >= 0; --i) {
            if (!widgetsCopy[(std::size_t)i]->onKeyRelease(key)) {
                return false;
            }
        }
        return true;
    }

    bool UIRenderer::onChar(char32_t unicodeCharacter) {
        if (unicodeCharacter > 0x00 && unicodeCharacter < 0xFF //accept 'Basic Latin' and 'Latin-1 Supplement'
                && unicodeCharacter > 0x1F //ignore 'Controls C0' unicode
                && (unicodeCharacter < 0x80 || unicodeCharacter > 0x9F) //ignore 'Controls C1' unicode
                && unicodeCharacter != 127 //ignore 'Delete' unicode
        ) {
            for (long i = (long) widgets.size() - 1; i >= 0; --i) {
                if (!widgets[(std::size_t) i]->onChar(unicodeCharacter)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool UIRenderer::onMouseMove(double mouseX, double mouseY) {
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            if (!widgets[(std::size_t)i]->onMouseMove((int)mouseX, (int)mouseY)) {
                return false;
            }
        }
        return true;
    }

    bool UIRenderer::onScroll(double offsetY) {
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            if (!widgets[(std::size_t)i]->onScroll(offsetY)) {
                return false;
            }
        }
        return true;
    }

    void UIRenderer::onDisable() {
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            widgets[(std::size_t)i]->onResetState();
        }
    }

    void UIRenderer::addWidget(const std::shared_ptr<Widget>& widget) {
        if (widget->getParent()) {
            throw std::runtime_error("Cannot add a widget having a parent to the UI renderer");
        }
        widgets.push_back(widget);

        if (cameraProjectionMatrix.has_value()) {
            widget->initialize(renderTarget, *uiShader, sceneSize, i18nService, cameraProjectionMatrix);
        } else {
            widget->initialize(renderTarget, *uiShader, Point2<unsigned int>(renderTarget.getWidth(), renderTarget.getHeight()), i18nService, cameraProjectionMatrix);
        }
        widget->addObserver(this, Widget::SET_IN_FOREGROUND);
    }

    void UIRenderer::removeWidget(Widget& widget) {
        auto itFind = std::find_if(widgets.begin(), widgets.end(), [&widget](const auto& o){return &widget == o.get();});
        if (itFind == widgets.end()) {
            throw std::runtime_error("The provided widget is not widget of this UI renderer");
        }
        widgets.erase(itFind);
    }

    void UIRenderer::removeAllWidgets() {
        widgets.clear();
    }

    const std::vector<std::shared_ptr<Widget>>& UIRenderer::getWidgets() const {
        return widgets;
    }

    void UIRenderer::prepareRendering(float dt, unsigned int& screenRenderingOrder) {
        prepareRendering(dt, screenRenderingOrder, Matrix4<float>());
    }

    void UIRenderer::prepareRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& viewMatrix) {
        ScopeProfiler sp(Profiler::graphic(), "uiPreRendering");

        float xScale = 0.005f; //TODO compute value
        float yFlipScale = -0.005f;
        Matrix4<float> uiViewMatrix( //TODO store in class ?
                xScale, 0.0f, 0.0f, 0.0f,
                0.0f, yFlipScale, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);

        Matrix4<float> viewModelMatrix = viewMatrix * uiViewMatrix * transform.getTransformMatrix();
        Matrix3<float> normalMatrix = transform.getOrientationMatrix().toMatrix3();
        for (auto& widget : widgets) {
            renderingOrder++;
            widget->prepareRendering(dt, renderingOrder, viewModelMatrix, normalMatrix);
        }

        //debug
        if (debugFont) {
            renderingOrder++;
            debugFont->prepareRendering(renderingOrder);
        }
    }

}
