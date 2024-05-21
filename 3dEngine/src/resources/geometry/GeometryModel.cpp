#include <resources/geometry/GeometryModel.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    GeometryModel::GeometryModel() :
            isInitialized(false),
            renderTarget(nullptr),
            color(Vector3<float>(0.0f, 1.0f, 0.0f)),
            polygonMode(PolygonMode::FILL),
            wireframeLineWidth(0.0075f),
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
        std::shared_ptr<GenericRendererBuilder> rendererBuilder;

        std::vector<uint32_t> indices;
        std::vector<Point3<float>> vertexArray = retrieveVertexArray(indices);

        if (polygonMode == PolygonMode::WIREFRAME) {
            if (getShapeType() != ShapeType::TRIANGLE) {
                throw std::runtime_error("Unsupported shape type for wireframe rendering: " + std::to_string((int)getShapeType()));
            }

            std::vector<LineSegment3D<float>> lines;
            if (!indices.empty()) {
                lines.reserve(3ul * indices.size());
                for (std::size_t i = 0; i < indices.size(); i+=3) {
                    lines.emplace_back(vertexArray[indices[i]], vertexArray[indices[i + 1]]);
                    lines.emplace_back(vertexArray[indices[i + 1]], vertexArray[indices[i + 2]]);
                    lines.emplace_back(vertexArray[indices[i + 2]], vertexArray[indices[i]]);
                }
            } else {
                lines.reserve(3ul * vertexArray.size());
                for (std::size_t i = 0; i < vertexArray.size(); i+=3) {
                    lines.emplace_back(vertexArray[i], vertexArray[i + 1]);
                    lines.emplace_back(vertexArray[i + 1], vertexArray[i + 2]);
                    lines.emplace_back(vertexArray[i + 2], vertexArray[i]);
                }
            }

            indices.clear();
            vertexArray = linesToVertexArray(lines, indices);
        }

        Matrix4<float> projectionViewModelMatrix;
        rendererBuilder = GenericRendererBuilder::create("geometry model", *renderTarget, *shader, getShapeType())
                ->addData(vertexArray)
                ->addUniformData(PVM_MATRIX_UNIFORM_BINDING, sizeof(projectionViewModelMatrix), &projectionViewModelMatrix)
                ->addUniformData(COLOR_UNIFORM_BINDING, sizeof(color), &color);
        if (!indices.empty()) {
            rendererBuilder->indices(indices);
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

    std::vector<Point3<float>> GeometryModel::linesToVertexArray(const std::vector<LineSegment3D<float>>& lines, std::vector<uint32_t>& indices) const {
        std::vector<Point3<float>> vertexArray;
        vertexArray.reserve(6ul * lines.size());

        float oneThirdRotation = AngleConverter<float>::toRadian(120);
        for (const auto& line : lines) {
            Vector3<float> lineVector = line.toVector().normalize();
            if (lineVector.squareLength() < 0.001f) {
                continue;
            }
            Quaternion<float> qRotation = Quaternion<float>::fromAxisAngle(lineVector, oneThirdRotation);

            Vector3<float> perpendicularVector1 = lineVector.perpendicularVector().normalize() * wireframeLineWidth;
            Vector3<float> perpendicularVector2 = qRotation.rotateVector(perpendicularVector1);
            Vector3<float> perpendicularVector3 = qRotation.rotateVector(perpendicularVector2);

            vertexArray.emplace_back(line.getA().translate(perpendicularVector1));
            vertexArray.emplace_back(line.getA().translate(perpendicularVector2));
            vertexArray.emplace_back(line.getA().translate(perpendicularVector3));

            vertexArray.emplace_back(line.getB().translate(perpendicularVector1));
            vertexArray.emplace_back(line.getB().translate(perpendicularVector2));
            vertexArray.emplace_back(line.getB().translate(perpendicularVector3));
        }

        indices.reserve(18ul * lines.size());
        for (uint32_t i = 0; i < lines.size(); ++i) {
            uint32_t startIndex = i * 6;

            std::array<uint32_t, 18> faceIndices = {
                //face 1
                startIndex + 3, startIndex + 1, startIndex + 0,
                startIndex + 1, startIndex + 3, startIndex + 4,
                //face 2
                startIndex + 4, startIndex + 2, startIndex + 1,
                startIndex + 2, startIndex + 4, startIndex + 5,
                //face 3
                startIndex + 5, startIndex + 0, startIndex + 2,
                startIndex + 0, startIndex + 5, startIndex + 3
            };
            indices.insert(indices.end(), std::begin(faceIndices), std::end(faceIndices));
        }

        return vertexArray;
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

    void GeometryModel::setWireframeLineWidth(float wireframeLineWidth) {
        this->wireframeLineWidth = wireframeLineWidth;
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
