#include "resources/geometry/GeometryModel.h"
#include "graphic/render/shader/builder/ShaderBuilder.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    GeometryModel::GeometryModel() :
            isInitialized(false),
            color(Vector4<float>(0.0f, 1.0f, 0.0f, 1.0f)),
            polygonMode(WIREFRAME),
            lineWidth(2.0f),
            pointSize(2.0f),
            transparencyEnabled(false),
            alwaysVisible(false) {
    }

    void GeometryModel::initialize(const std::shared_ptr<RenderTarget>& renderTarget) {
        this->renderTarget = renderTarget;

        modelMatrix = retrieveModelMatrix();
        refreshRenderer();

        isInitialized = true;
    }

    void GeometryModel::refreshRenderer() {
        if(!renderTarget) {
            return;
        }

        std::vector<Point3<float>> vertexArray = retrieveVertexArray();

        std::vector<std::size_t> variablesSize = {sizeof(pointSize)};
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &pointSize);
        shader = ShaderBuilder::createShader("displayGeometry.vert.spv", "", "displayGeometry.frag.spv", std::move(shaderConstants));

        auto rendererBuilder = GenericRendererBuilder::create("geometry model", renderTarget, shader, getShapeType())
                ->addData(vertexArray)
                ->addUniformData(sizeof(positioningData), &positioningData) //binding 0
                ->addUniformData(sizeof(color), &color) //binding 1
                ->disableCullFace()
                ->lineWidth(lineWidth)
                ->polygonMode(polygonMode);

        if (!alwaysVisible) {
            rendererBuilder->enableDepthOperations();
        }

        if (transparencyEnabled) {
            rendererBuilder->enableTransparency();
        }

        renderer = rendererBuilder->build();
    }

    void GeometryModel::onCameraProjectionUpdate(const Matrix4<float>& projectionMatrix) {
        positioningData.projectionMatrix = projectionMatrix;
    }

    const std::shared_ptr<RenderTarget>& GeometryModel::getRenderTarget() const {
        return renderTarget;
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

    void GeometryModel::setLineWidth(float lineWidth) {
        this->lineWidth = lineWidth;
        refreshRenderer();
    }

    void GeometryModel::setPointSize(float pointSize) {
        this->pointSize = pointSize;
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

    void GeometryModel::prepareRendering(const Matrix4<float>& viewMatrix) const {
        if (!isInitialized) {
            throw std::runtime_error("Geometry model must be initialized before call prepare rendering");
        }

        positioningData.viewModelMatrix = viewMatrix * modelMatrix;
        renderer->updateUniformData(0, &positioningData);
        renderer->updateUniformData(1, &color);
    }

}
