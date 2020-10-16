#include <GL/glew.h>
#include <utility>

#include "LinesModel.h"

namespace urchin {

    LinesModel::LinesModel(std::vector<Point3<float>> stripLinesPoints, float linesSize) :
            linesPoints(std::move(stripLinesPoints)),
            stripLines(true),
            linesSize(linesSize) {
        initialize();
    }

    LinesModel::LinesModel(const std::vector<LineSegment3D<float>> &lineSegments, float linesSize) :
            stripLines(false),
            linesSize(linesSize) {
        linesPoints.reserve(lineSegments.size() * 2);
        for (const auto &lineSegment : lineSegments) {
            linesPoints.emplace_back(lineSegment.getA());
            linesPoints.emplace_back(lineSegment.getB());
        }

        initialize();
    }

    LinesModel::LinesModel(const LineSegment3D<float> &lineSegment, float linesSize) :
        stripLines(true),
        linesSize(linesSize) {
        linesPoints.push_back(lineSegment.getA());
        linesPoints.push_back(lineSegment.getB());

        initialize();
    }

    Matrix4<float> LinesModel::retrieveModelMatrix() const {
        return {};
    }

    std::vector<Point3<float>> LinesModel::retrieveVertexArray() const {
        return linesPoints;
    }

    void LinesModel::drawGeometry() const {
        GLfloat savedLineWidth;
        glGetFloatv(GL_LINE_WIDTH, &savedLineWidth);
        glLineWidth(linesSize);

        glDrawArrays(stripLines ? GL_LINE_STRIP : GL_LINES, 0, linesPoints.size());

        glLineWidth(savedLineWidth);
    }
}
