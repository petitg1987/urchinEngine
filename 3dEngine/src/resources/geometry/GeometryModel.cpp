#include "resources/geometry/GeometryModel.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    GeometryModel::GeometryModel() :
            color(Vector4<float>(0.0f, 1.0f, 0.0f, 1.0f)),
            polygonMode(WIREFRAME),
            outlineSize(1.3f),
            transparencyEnabled(false),
            alwaysVisible(false) {
        shader = ShaderBuilder().createShader("displayGeometry.vert", "", "displayGeometry.frag");

        mProjectionShaderVar = ShaderVar(shader, "mProjection");
        mViewShaderVar = ShaderVar(shader, "mView");
        colorShaderVar = ShaderVar(shader, "color");
    }

    void GeometryModel::initialize() {
        modelMatrix = retrieveModelMatrix();
        refreshRenderer();
    }

    void GeometryModel::refreshRenderer() {
        std::vector<Point3<float>> vertexArray = retrieveVertexArray();

        std::unique_ptr<GenericRendererBuilder> rendererBuilder = std::make_unique<GenericRendererBuilder>(getShapeType());
        rendererBuilder
                ->vertexCoord(&vertexArray)
                ->disableCullFace()
                ->outlineSize(outlineSize)
                ->polygonMode(polygonMode);

        if(!alwaysVisible) {
            rendererBuilder->enableDepthTest();
        }

        if(transparencyEnabled) {
            rendererBuilder->enableTransparency();
        }

        renderer = rendererBuilder->build();
    }

    void GeometryModel::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix) {
        this->projectionMatrix = projectionMatrix;
    }

    Vector4<float> GeometryModel::getColor() const {
        return color;
    }

    void GeometryModel::setColor(float red, float green, float blue, float alpha) {
        this->color = Vector4<float>(red, green, blue, alpha);
    }

    PolygonMode GeometryModel::getPolygonMode() const {
        return polygonMode;
    }

    void GeometryModel::setPolygonMode(PolygonMode polygonMode) {
        this->polygonMode = polygonMode;
        refreshRenderer();
    }

    void GeometryModel::setOutlineSize(float outlineSize) {
        this->outlineSize = outlineSize;
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

    void GeometryModel::display(const Matrix4<float> &viewMatrix) const {
        ShaderDataSender()
            .sendData(mProjectionShaderVar, projectionMatrix)
            .sendData(mViewShaderVar, viewMatrix * modelMatrix)
            .sendData(colorShaderVar, color);

        shader->bind();
        renderer->draw();
    }

}
