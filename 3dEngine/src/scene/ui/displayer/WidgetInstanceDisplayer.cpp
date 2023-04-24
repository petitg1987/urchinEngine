#include <scene/ui/displayer/WidgetInstanceDisplayer.h>
#include <scene/ui/UIRenderer.h>

namespace urchin {

    WidgetInstanceDisplayer::WidgetInstanceDisplayer(const UIRenderer& uiRenderer) :
            isInitialized(false),
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

    void WidgetInstanceDisplayer::initialize(std::shared_ptr<Texture> texture) {
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

        colorParams.alphaFactor = getReferenceWidget().getAlphaFactor(); //TODO /!\ if updated => new instance
        colorParams.gammaFactor = uiRenderer.getGammaFactor();
        rendererBuilder->addUniformData(sizeof(colorParams), &colorParams); //binding 2

        rendererBuilder->addData(getReferenceWidget().retrieveVertexCoordinates()); //TODO update on updateSize => new instance
        rendererBuilder->addData(getReferenceWidget().retrieveTextureCoordinates()); //TODO update on updateSize => new instance

        instanceModelMatrices.emplace_back();
        rendererBuilder->instanceData(instanceModelMatrices.size(), VariableType::MAT4, (const float*)instanceModelMatrices.data());

        std::optional<Scissor> scissor = getReferenceWidget().retrieveScissor(); //TODO update at init (when parent is known) & onResize &
        if (scissor.has_value()) {
            rendererBuilder->setScissor(scissor.value().getScissorOffset(), scissor.value().getScissorSize());
        }

        rendererBuilder->addUniformTextureReader(TextureReader::build(std::move(texture), TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))); //binding 3

        renderer = rendererBuilder->build();

        isInitialized = true;
    }

    void WidgetInstanceDisplayer::updateTexture(std::shared_ptr<Texture> texture) {
        //TODO add debug instanceID check still valid !?
        renderer->updateUniformTextureReader(0, TextureReader::build(std::move(texture), TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));
    }

    Widget& WidgetInstanceDisplayer::getReferenceWidget() const {
        //A reference widget is a widget which can be used to represent all instance widgets.
        if (instanceWidgets.empty()) {
            throw std::runtime_error("No reference widget on bared widget displayer");
        }
        return *instanceWidgets[0];
    }

    unsigned int WidgetInstanceDisplayer::computeDepthLevel(Widget& widget) const {
        unsigned int depthLevel = 0;
        const Widget* currentParent = widget.getParent();
        while (currentParent != nullptr) {
            depthLevel++;
            currentParent = currentParent->getParent();
        }
        return depthLevel;
    }

    TextureParam::Anisotropy WidgetInstanceDisplayer::getTextureAnisotropy() const {
        if (uiRenderer.getUi3dData()) {
            return TextureParam::Anisotropy::ANISOTROPY;
        }
        return TextureParam::Anisotropy::NO_ANISOTROPY;
    }

    void WidgetInstanceDisplayer::addInstanceWidget(Widget& widget) {
        //TODO assert uiRenderer are equals between this class and the widget
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

//TODO        model.addObserver(this, Model::MESH_VERTICES_UPDATED);
//        model.addObserver(this, Model::MESH_UV_UPDATED);
//        model.addObserver(this, Model::MATERIAL_UPDATED);
//        model.addObserver(this, Model::SCALE_UPDATED);
    }

    void WidgetInstanceDisplayer::removeInstanceWidget(Widget& widget) {
        widget.detachWidgetInstanceDisplayer(*this);
        std::size_t erasedCount = std::erase_if(instanceWidgets, [&widget](const Widget* w) {return w == &widget;});
        if (erasedCount != 1) {
            Logger::instance().logError("Removing the instance widget fail");
        }

//TODO        model.removeObserver(this, Model::SCALE_UPDATED);
//        model.removeObserver(this, Model::MATERIAL_UPDATED);
//        model.removeObserver(this, Model::MESH_UV_UPDATED);
//        model.removeObserver(this, Model::MESH_VERTICES_UPDATED);
    }

    unsigned int WidgetInstanceDisplayer::getInstanceCount() const {
        return (unsigned int)instanceWidgets.size();
    }

    void WidgetInstanceDisplayer::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix) const {
        float zBias = 0.0f;
        if (uiRenderer.getUi3dData()) {
            float squareDistanceUiToCamera = uiRenderer.getUi3dData()->uiPosition.squareDistance(uiRenderer.getUi3dData()->camera->getPosition());
            zBias = (float)computeDepthLevel(getReferenceWidget()) * 0.0003f * std::clamp(squareDistanceUiToCamera, 0.5f, 6.0f);
            Matrix4<float> uiProjectionViewMatrix = projectionViewMatrix * uiRenderer.getUi3dData()->modelMatrix;
            renderer->updateUniformData(1, &uiProjectionViewMatrix);
        }

        instanceModelMatrices.clear();
        for (const Widget* widget : instanceWidgets) {
            //Equivalent to 4 multiplied matrices: D * C * B * A
            // A) Translation of the widget center to the origin (transOriginX, transOriginY)
            // B) Scale widget (scale.X, scale.Y)
            // C) Rotate widget (sinRotate, cosRotate)
            // D) Translation rollback of "a" + translation for positioning (transX, transY, zBias)
            float transX = widget->getGlobalPositionX() + widget->getWidth() / 2.0f;
            float transY = widget->getGlobalPositionY() + widget->getHeight() / 2.0f;
            float transOriginX = -widget->getWidth() / 2.0f;
            float transOriginY = -widget->getHeight() / 2.0f;
            float sinRotate = std::sin(widget->getRotation());
            float cosRotate = std::cos(widget->getRotation());
            Matrix4<float> modelMatrix(
                    widget->getScale().X * cosRotate, widget->getScale().Y * -sinRotate, 0.0f, transX + (transOriginX * widget->getScale().X * cosRotate) + (transOriginY * widget->getScale().Y * -sinRotate),
                    widget->getScale().X * sinRotate, widget->getScale().Y * cosRotate, 0.0f, transY + (transOriginX * widget->getScale().X * sinRotate) + (transOriginY * widget->getScale().Y * cosRotate),
                    0.0f, 0.0, 1.0f, zBias,
                    0.0f, 0.0f, 0.0f, 1.0f);
            instanceModelMatrices.emplace_back(modelMatrix);
        }
        renderer->updateInstanceData(getInstanceCount(), (const float*)instanceModelMatrices.data());

        renderer->enableRenderer(renderingOrder);
    }

}
