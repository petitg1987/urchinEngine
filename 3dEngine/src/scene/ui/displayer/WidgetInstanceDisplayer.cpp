#include <scene/ui/displayer/WidgetInstanceDisplayer.h>
#include <scene/ui/UIRenderer.h>

namespace urchin {

    WidgetInstanceDisplayer::WidgetInstanceDisplayer(UIRenderer& uiRenderer, RenderTarget& renderTarget, const Shader& shader) :
            isInitialized(false),
            instanceId(WidgetDisplayable::INSTANCING_DENY_ID),
            uiRenderer(uiRenderer),
            renderTarget(renderTarget),
            shader(shader) {

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

        auto rendererBuilder = GenericRendererBuilder::create(std::move(name), renderTarget, shader, ShapeType::TRIANGLE);

        Matrix4<float> normalMatrix;
        Matrix4<float> projectionViewModelMatrix;
        Matrix4<float> modelMatrix;
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
            if (hasTransparency) {
                rendererBuilder->enableTransparency({BlendFunction::buildDefault()});
            }
            projectionViewModelMatrix = Matrix4<float>( //orthogonal matrix with origin at top left screen
                    2.0f / (float) uiRenderer.getUiResolution().X, 0.0f, -1.0f, 0.0f,
                    0.0f, 2.0f / (float) uiRenderer.getUiResolution().Y, -1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
        }

        rendererBuilder->addUniformData(sizeof(normalMatrix), &normalMatrix); //binding 0
        rendererBuilder->addUniformData(sizeof(projectionViewModelMatrix), &projectionViewModelMatrix); //binding 1

        colorParams.gammaFactor = uiRenderer.getGammaFactor();
        rendererBuilder->addUniformData(sizeof(colorParams), &colorParams); //binding 2

        refreshCoordinates();
        rendererBuilder->addData(vertexCoord);
        rendererBuilder->addData(textureCoord);
        rendererBuilder->instanceData(1, VariableType::MAT4, (const float*)&modelMatrix);

        refreshScissor(true);
        if (scissorEnabled) {
            rendererBuilder->setScissor(scissorOffset, scissorSize);
        }

        //TODO add stuff specific to widget implementation (e.g. texture)

        widgetRenderer = rendererBuilder->build();

        isInitialized = true;
    }

    Widget& WidgetInstanceDisplayer::getReferenceWidget() const {
        //A reference widget is a widget which can be used to represent all instance widgets.
        if (instanceWidgets.empty()) {
            throw std::runtime_error("No reference widget on bared widget displayer");
        }
        return *instanceWidgets[0];
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
            zBias = (float) computeDepthLevel() * 0.0003f * std::clamp(squareDistanceUiToCamera, 0.5f, 6.0f); //TODO different parent => no instance
            Matrix4<float> uiProjectionViewMatrix = projectionViewMatrix * uiRenderer.getUi3dData()->modelMatrix;
            renderer->updateUniformData(1, &uiProjectionViewMatrix);
        }

        //Equivalent to 4 multiplied matrices: D * C * B * A
        // A) Translation of the widget center to the origin (transOriginX, transOriginY)
        // B) Scale widget (scale.X, scale.Y)
        // C) Rotate widget (sinRotate, cosRotate)
        // D) Translation rollback of "a" + translation for positioning (transX, transY, zBias)
        Vector2<float> translateVector(getGlobalPositionX(), getGlobalPositionY());
        float transX = translateVector.X + getWidth() / 2.0f;
        float transY = translateVector.Y + getHeight() / 2.0f;
        float transOriginX = -getWidth() / 2.0f;
        float transOriginY = -getHeight() / 2.0f;
        float sinRotate = std::sin(rotationZ);
        float cosRotate = std::cos(rotationZ);
        Matrix4<float> modelMatrix(
                scale.X * cosRotate, scale.Y * -sinRotate, 0.0f, transX + (transOriginX * scale.X * cosRotate) + (transOriginY * scale.Y * -sinRotate),
                scale.X * sinRotate, scale.Y * cosRotate, 0.0f, transY + (transOriginX * scale.X * sinRotate) + (transOriginY * scale.Y * cosRotate),
                0.0f, 0.0, 1.0f, zBias,
                0.0f, 0.0f, 0.0f, 1.0f);
        renderer->updateInstanceData(1, (const float*)&modelMatrix);

        renderer->enableRenderer(renderingOrder);
    }

}
