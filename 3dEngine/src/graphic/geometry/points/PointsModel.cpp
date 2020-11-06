#include <GL/glew.h>

#include <utility>

#include "graphic/geometry/points/PointsModel.h"

namespace urchin {

    PointsModel::PointsModel(std::vector<Point3<float>> points, float pointsSize):
            points(std::move(points)),
            pointsSize(pointsSize) {
        initialize();
    }

    PointsModel::PointsModel(const Point3<float> &point, float pointsSize):
        pointsSize(pointsSize) {
        points.push_back(point);

        initialize();
    }

    Matrix4<float> PointsModel::retrieveModelMatrix() const {
        return {};
    }

    std::vector<Point3<float>> PointsModel::retrieveVertexArray() const {
        return points;
    }

    void PointsModel::drawGeometry() const {
        GLfloat savedPointSize;
        glGetFloatv(GL_POINT_SIZE, &savedPointSize);
        glPointSize(pointsSize);

        glDrawArrays(GL_POINTS, 0, points.size());

        glPointSize(savedPointSize);
    }
}
