#include <scene/ui/displayer/WidgetInstanceDisplayer.h>
#include <scene/ui/displayer/WidgetSetDisplayer.h>
#include <scene/ui/UIRenderer.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    WidgetInstanceDisplayer::WidgetInstanceDisplayer(const WidgetSetDisplayer& widgetSetDisplayer, const UIRenderer& uiRenderer) :
            isInitialized(false),
            widgetSetDisplayer(widgetSetDisplayer),
            instanceId(WidgetDisplayable::INSTANCING_DENY_ID),
            uiRenderer(uiRenderer),
            colorParams({}) {

    }

    WidgetInstanceDisplayer::~WidgetInstanceDisplayer() {
        std::vector<Widget*> copiedInstanceWidgets = instanceWidgets;
        for (Widget* widget : copiedInstanceWidgets) {
            removeInstanceWidget(*widget);
        }
    }

    void WidgetInstanceDisplayer::initialize() {
        if (isInitialized) {
            throw std::runtime_error("Widget displayer is already initialized");
        } else if (instanceWidgets.empty()) {
            throw std::runtime_error("At least one instance widget must be added before initialization");
        }

        auto rendererBuilder = GenericRendererBuilder::create("widget_" + std::to_string((int)getReferenceWidget().getWidgetType()), uiRenderer.getRenderTarget(), uiRenderer.getShader(), ShapeType::TRIANGLE);

        Matrix4<float> normalMatrix;
        Matrix4<float> projectionViewModelMatrix;
        if (uiRenderer.getUi3dData()) {
            rendererBuilder->enableDepthTest();
            rendererBuilder->enableDepthWrite();

            //Always active transparency to ensure that emissive factor stay unchanged (see fragment shader for more details).
            //Transparency is only working when a transparent widget is displayed above another widget.
            //Transparency to the scene is currently not supported because the UI is written on an RGB channel (no alpha).
            rendererBuilder->enableTransparency({
                    BlendFunction::build(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ZERO, BlendFactor::ONE),
                    BlendFunction::buildBlendDisabled(),
                    BlendFunction::buildBlendDisabled()
            });
            normalMatrix = uiRenderer.getUi3dData()->normalMatrix;
        } else {
            rendererBuilder->enableTransparency({BlendFunction::buildDefault()});
            projectionViewModelMatrix = Matrix4<float>( //orthogonal matrix with origin at top left screen
                    2.0f / (float) uiRenderer.getUiResolution().X, 0.0f, -1.0f, 0.0f,
                    0.0f, 2.0f / (float) uiRenderer.getUiResolution().Y, -1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
        }

        rendererBuilder->addUniformData(sizeof(normalMatrix), &normalMatrix); //binding 0
        rendererBuilder->addUniformData(sizeof(projectionViewModelMatrix), &projectionViewModelMatrix); //binding 1

        colorParams.alphaFactor = getReferenceWidget().getAlphaFactor();
        colorParams.gammaFactor = uiRenderer.getGammaFactor();
        rendererBuilder->addUniformData(sizeof(colorParams), &colorParams); //binding 2

        coordinates.clear();
        getReferenceWidget().retrieveVertexCoordinates(coordinates);
        rendererBuilder->addData(coordinates);

        coordinates.clear();
        getReferenceWidget().retrieveTextureCoordinates(coordinates);
        rendererBuilder->addData(coordinates);

        instanceModelMatrices.emplace_back();
        rendererBuilder->instanceData(instanceModelMatrices.size(), VariableType::MAT4, (const float*)instanceModelMatrices.data());

        std::optional<Scissor> scissor = getReferenceWidget().retrieveScissor();
        if (scissor.has_value()) {
            rendererBuilder->setScissor(scissor.value().getScissorOffset(), scissor.value().getScissorSize());
        }

        rendererBuilder->addUniformTextureReader(TextureReader::build(getReferenceWidget().getTexture(), TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))); //binding 3

        renderer = rendererBuilder->build();

        isInitialized = true;
    }

    void WidgetInstanceDisplayer::notify(Observable* observable, int notificationType) {
        if (const auto* widget = dynamic_cast<Widget*>(observable)) {
            if (notificationType == Widget::TEXTURE_UPDATED) {
                updateTexture(widget);
            } else if (notificationType == Widget::SIZE_UPDATED) {
                updateCoordinates(widget);
                updateScissor(widget);
            } else if (notificationType == Widget::POSITION_UPDATED) {
                updateScissor(widget);
            } else if (notificationType == Widget::ALPHA_FACTOR_UPDATED) {
                updateAlphaFactor(widget);
            }
        }
    }

    void WidgetInstanceDisplayer::onUiRendererSizeUpdated() {
        if (!uiRenderer.getUi3dData()) {
            Matrix4<float> projectionViewModelMatrix( //orthogonal matrix with origin at top left screen
                    2.0f / (float) uiRenderer.getUiResolution().X, 0.0f, -1.0f, 0.0f,
                    0.0f, 2.0f / (float) uiRenderer.getUiResolution().Y, -1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
            renderer->updateUniformData(1, &projectionViewModelMatrix);
        }
    }

    void WidgetInstanceDisplayer::onGammaFactorUpdated() {
        colorParams.gammaFactor = uiRenderer.getGammaFactor();
        renderer->updateUniformData(2, &colorParams);
    }

    const WidgetSetDisplayer& WidgetInstanceDisplayer::getWidgetSetDisplayer() const {
        return widgetSetDisplayer;
    }

    std::size_t WidgetInstanceDisplayer::getInstanceId() const {
        return instanceId;
    }

    Widget& WidgetInstanceDisplayer::getReferenceWidget() const {
        //A reference widget is a widget which can be used to represent all instance widgets.
        if (instanceWidgets.empty()) {
            throw std::runtime_error("No reference widget on bared widget displayer");
        }
        return *instanceWidgets[0];
    }

    TextureParam::Anisotropy WidgetInstanceDisplayer::getTextureAnisotropy() const {
        if (uiRenderer.getUi3dData()) {
            return TextureParam::Anisotropy::ANISOTROPY;
        }
        return TextureParam::Anisotropy::NO_ANISOTROPY;
    }

    void WidgetInstanceDisplayer::addInstanceWidget(Widget& widget) {
        if (instanceWidgets.empty()) {
            instanceId = widget.computeInstanceId();
        } else {
            #ifdef URCHIN_DEBUG
                assert(instanceId != WidgetDisplayable::INSTANCING_DENY_ID);
                assert(instanceId == widget.computeInstanceId());
            #endif
        }

        instanceWidgets.push_back(&widget);
        widget.attachWidgetInstanceDisplayer(*this);

        widget.addObserver(this, Widget::TEXTURE_UPDATED);
        widget.addObserver(this, Widget::SIZE_UPDATED);
        widget.addObserver(this, Widget::POSITION_UPDATED);
        widget.addObserver(this, Widget::ALPHA_FACTOR_UPDATED);
    }

    void WidgetInstanceDisplayer::removeInstanceWidget(Widget& widget) {
        widget.detachWidgetInstanceDisplayer(*this);
        std::size_t erasedCount = std::erase_if(instanceWidgets, [&widget](const Widget* w) {return w == &widget;});
        if (erasedCount != 1) {
            Logger::instance().logError("Removing the instance widget fail");
        }

        widget.removeObserver(this, Widget::ALPHA_FACTOR_UPDATED);
        widget.removeObserver(this, Widget::POSITION_UPDATED);
        widget.removeObserver(this, Widget::SIZE_UPDATED);
        widget.removeObserver(this, Widget::TEXTURE_UPDATED);
    }

    unsigned int WidgetInstanceDisplayer::getInstanceCount() const {
        return (unsigned int)instanceWidgets.size();
    }

    bool WidgetInstanceDisplayer::checkUpdateAllowance(const Widget* widget) const {
        bool canUpdateDisplayer = instanceId == WidgetDisplayable::INSTANCING_DENY_ID;
        if (!canUpdateDisplayer && widget->computeInstanceId() == instanceId) {
            //Update without impact on the instance ID, either:
            // - Update has no effect. Example: change size from 1.0f to 1.0f
            // - Bug in the way the instance ID is computed
            return true;
        }
        return canUpdateDisplayer;
    }

    void WidgetInstanceDisplayer::updateTexture(const Widget* widget) {
        if (checkUpdateAllowance(widget)) {
            renderer->updateUniformTextureReader(0, TextureReader::build(getReferenceWidget().getTexture(), TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));
        }
    }

    void WidgetInstanceDisplayer::updateScissor(const Widget* widget) {
        if (checkUpdateAllowance(widget)) {
            std::optional<Scissor> scissor = getReferenceWidget().retrieveScissor();
            if (scissor.has_value()) {
                renderer->updateScissor(scissor.value().getScissorOffset(), scissor.value().getScissorSize());
            }
        }
    }

    void WidgetInstanceDisplayer::updateCoordinates(const Widget* widget) {
        if (checkUpdateAllowance(widget)) {
            coordinates.clear();
            getReferenceWidget().retrieveVertexCoordinates(coordinates);
            renderer->updateData(0, coordinates);

            coordinates.clear();
            getReferenceWidget().retrieveTextureCoordinates(coordinates);
            renderer->updateData(1, coordinates);
        }
    }

    void WidgetInstanceDisplayer::updateAlphaFactor(const Widget* widget) {
        if (checkUpdateAllowance(widget)) {
            colorParams.alphaFactor = getReferenceWidget().getAlphaFactor();
            renderer->updateUniformData(2, &colorParams);
        }
    }

    void WidgetInstanceDisplayer::resetRenderingWidgets() {
        instanceModelMatrices.clear();
    }

    void WidgetInstanceDisplayer::registerRenderingWidget(const Widget& widget) {
        #ifdef URCHIN_DEBUG
            assert(widget.computeInstanceId() == instanceId);
        #endif

        float zBias = 0.0f;
        if (uiRenderer.getUi3dData()) {
            float squareDistanceUiToCamera = uiRenderer.getUi3dData()->uiPosition.squareDistance(uiRenderer.getUi3dData()->camera->getPosition());
            zBias = (float)getReferenceWidget().computeDepthLevel() * 0.0003f * std::clamp(squareDistanceUiToCamera, 0.5f, 6.0f);
        }

        //Equivalent to 4 multiplied matrices: D * C * B * A
        // A) Translation of the widget center to the origin (transOriginX, transOriginY)
        // B) Scale widget (scale.X, scale.Y)
        // C) Rotate widget (sinRotate, cosRotate)
        // D) Translation rollback of "a" + translation for positioning (transX, transY, zBias)
        float transX = widget.getGlobalPositionX() + widget.getWidth() / 2.0f;
        float transY = widget.getGlobalPositionY() + widget.getHeight() / 2.0f;
        float transOriginX = -widget.getWidth() / 2.0f;
        float transOriginY = -widget.getHeight() / 2.0f;
        float sinRotate = std::sin(widget.getRotation());
        float cosRotate = std::cos(widget.getRotation());
        Matrix4<float> modelMatrix(
                widget.getScale().X * cosRotate, widget.getScale().Y * -sinRotate, 0.0f, transX + (transOriginX * widget.getScale().X * cosRotate) + (transOriginY * widget.getScale().Y * -sinRotate),
                widget.getScale().X * sinRotate, widget.getScale().Y * cosRotate, 0.0f, transY + (transOriginX * widget.getScale().X * sinRotate) + (transOriginY * widget.getScale().Y * cosRotate),
                0.0f, 0.0, 1.0f, zBias,
                0.0f, 0.0f, 0.0f, 1.0f);
        instanceModelMatrices.emplace_back(modelMatrix);
    }

    void WidgetInstanceDisplayer::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix) const {
        if (uiRenderer.getUi3dData()) {
            Matrix4<float> uiProjectionViewMatrix = projectionViewMatrix * uiRenderer.getUi3dData()->modelMatrix;
            renderer->updateUniformData(1, &uiProjectionViewMatrix);
        }

        renderer->updateInstanceData(instanceModelMatrices.size(), (const float*)instanceModelMatrices.data());
        renderer->enableRenderer(renderingOrder);
    }

}
