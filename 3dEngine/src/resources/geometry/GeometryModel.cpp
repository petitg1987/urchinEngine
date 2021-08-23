#include <resources/geometry/GeometryModel.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    GeometryModel::GeometryModel() :
            isInitialized(false),
            renderTarget(nullptr),
            color(Vector4<float>(0.0f, 1.0f, 0.0f, 1.0f)),
            polygonMode(WIREFRAME),
            transparencyEnabled(false),
            alwaysVisible(false) {
    }

    void GeometryModel::initialize(RenderTarget& renderTarget) {
        this->renderTarget = &renderTarget;
        refreshRenderer();

        isInitialized = true;
    }

    void GeometryModel::refreshRenderer() {
        if (!renderTarget) {
            return;
        }

        shader = ShaderBuilder::createShader("displayGeometry.vert.spv", "", "displayGeometry.frag.spv");

        std::vector<Point3<float>> vertexArray = retrieveVertexArray();
        auto rendererBuilder = GenericRendererBuilder::create("geometry model", *renderTarget, *shader, getShapeType())
                ->addData(vertexArray)
                ->addUniformData(sizeof(positioningData), &positioningData) //binding 0
                ->addUniformData(sizeof(color), &color) //binding 1
                ->polygonMode(polygonMode);

        if (polygonMode == PolygonMode::WIREFRAME) {
            rendererBuilder->disableCullFace();
        }

        if (!alwaysVisible) {
            rendererBuilder->enableDepthTest();
            rendererBuilder->enableDepthWrite();
        }

        if (transparencyEnabled) {
            std::vector<BlendFunction> blendFunctions;
            blendFunctions.resize(renderTarget->getNumColorAttachment(), BlendFunction::buildDefault());
            rendererBuilder->enableTransparency(blendFunctions);
        }

        renderer = rendererBuilder->build();
    }

    void GeometryModel::onCameraProjectionUpdate(const Matrix4<float>& projectionMatrix) {
        positioningData.projectionMatrix = projectionMatrix;
    }

    const RenderTarget& GeometryModel::getRenderTarget() const {
        return *renderTarget;
    }

    Vector4<float> GeometryModel::getColor() const {
        return color;
    }

    void GeometryModel::setColor(float red, float green, float blue, float alpha) {
        color = Vector4<float>(red, green, blue, alpha);
    }

    PolygonMode GeometryModel::getPolygonMode() const {
        return polygonMode;
    }

    void GeometryModel::setPolygonMode(PolygonMode polygonMode) {
        this->polygonMode = polygonMode;
        refreshRenderer();
    }

    bool GeometryModel::isTransparencyEnabled() const {
        return transparencyEnabled;
    }

    void GeometryModel::enableTransparency() {
        transparencyEnabled = true;
        refreshRenderer();
    }

    bool GeometryModel::isAlwaysVisible() const {
        return alwaysVisible;
    }

    void GeometryModel::setAlwaysVisible(bool alwaysVisible) {
        this->alwaysVisible = alwaysVisible;
        refreshRenderer();
    }

    void GeometryModel::setModelMatrix(const Matrix4<float>& modelMatrix) {
        this->modelMatrix = modelMatrix;
    }

    void GeometryModel::prepareRendering(const Matrix4<float>& viewMatrix) const {
        if (!isInitialized) {
            throw std::runtime_error("Geometry model must be initialized before call prepare rendering");
        }

        positioningData.viewModelMatrix = viewMatrix * modelMatrix;
        renderer->updateUniformData(0, &positioningData);
        renderer->updateUniformData(1, &color);
        if (!renderer->isEnabled()) { //renderer could be already enabled if 'initialize' method is called after 'renderTarget->disableAllRenderers()'
            renderer->enableRenderer();
        }
    }

}
