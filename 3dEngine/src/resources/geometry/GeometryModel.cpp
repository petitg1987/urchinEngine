#include <resources/geometry/GeometryModel.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    GeometryModel::GeometryModel() :
            isInitialized(false),
            renderTarget(nullptr),
            color(Vector3<float>(0.0f, 1.0f, 0.0f)),
            polygonMode(PolygonMode::FILL),
            alwaysVisible(false),
            cullFaceDisabled(false) {
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

        std::vector<uint32_t> indices;
        std::vector<Point3<float>> vertexArray = retrieveVertexArray(indices);
        Matrix4<float> projectionViewModelMatrix;
        auto rendererBuilder = GenericRendererBuilder::create("geometry model", *renderTarget, *shader, getShapeType())
                ->addData(vertexArray)
                ->addUniformData(PVM_MATRIX_UNIFORM_BINDING, sizeof(projectionViewModelMatrix), &projectionViewModelMatrix)
                ->addUniformData(COLOR_UNIFORM_BINDING, sizeof(color), &color)
                ->polygonMode(polygonMode);

        if (!indices.empty()) {
            rendererBuilder->indices(indices);
        }

        if (polygonMode == PolygonMode::WIREFRAME) {
            rendererBuilder->disableCullFace();
        }

        if (!alwaysVisible) {
            rendererBuilder->enableDepthTest();
            rendererBuilder->enableDepthWrite();
        }

        if (cullFaceDisabled) {
            rendererBuilder->disableCullFace();
        }

        renderer = rendererBuilder->build();
        renderer->disableRenderer(); //in case this method is called after 'renderTarget->disableAllRenderers()'
    }

    const RenderTarget& GeometryModel::getRenderTarget() const {
        return *renderTarget;
    }

    Vector3<float> GeometryModel::getColor() const {
        return color;
    }

    void GeometryModel::setColor(float red, float green, float blue) {
        color = Vector3<float>(red, green, blue);
        if (renderer) {
            renderer->updateUniformData(COLOR_UNIFORM_BINDING, &color);
        }
    }

    PolygonMode GeometryModel::getPolygonMode() const {
        return polygonMode;
    }

    void GeometryModel::setPolygonMode(PolygonMode polygonMode) {
        this->polygonMode = polygonMode;
        refreshRenderer();
    }

    bool GeometryModel::isAlwaysVisible() const {
        return alwaysVisible;
    }

    void GeometryModel::setAlwaysVisible(bool alwaysVisible) {
        this->alwaysVisible = alwaysVisible;
        refreshRenderer();
    }

    bool GeometryModel::isCullFaceDisabled() const {
        return cullFaceDisabled;
    }

    void GeometryModel::disableCullFace() {
        this->cullFaceDisabled = true;
        refreshRenderer();
    }

    void GeometryModel::setModelMatrix(const Matrix4<float>& modelMatrix) {
        this->modelMatrix = modelMatrix;
    }

    void GeometryModel::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix) const {
        if (!isInitialized) {
            throw std::runtime_error("Geometry model must be initialized before call prepare rendering");
        }

        Matrix4<float> projectionViewModelMatrix = projectionViewMatrix * modelMatrix;
        renderer->updateUniformData(PVM_MATRIX_UNIFORM_BINDING, &projectionViewModelMatrix);
        renderer->enableRenderer(renderingOrder);
    }

}
