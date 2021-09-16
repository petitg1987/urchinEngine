#include <cassert>
#include <limits>
#include <cmath>
#include <stdexcept>

#include <math/geometry/3d/object/OBBox.h>
#include <math/algebra/point/Point4.h>

namespace urchin {

    template<class T> OBBox<T>::OBBox():
            halfSizes(Vector3<T>(0.0, 0.0, 0.0)),
            centerOfMass(Point3<T>(0.0, 0.0, 0.0)) {
        axis[0] = Vector3<T>(0.0, 0.0, 0.0);
        axis[1] = Vector3<T>(0.0, 0.0, 0.0);
        axis[2] = Vector3<T>(0.0, 0.0, 0.0);

        normalizedAxis[0] = Vector3<T>(0.0, 0.0, 0.0);
        normalizedAxis[1] = Vector3<T>(0.0, 0.0, 0.0);
        normalizedAxis[2] = Vector3<T>(0.0, 0.0, 0.0);
    }

    template<class T> OBBox<T>::OBBox(const Vector3<T>& halfSizes, const Point3<T>& centerOfMass, const Quaternion<T>& orientation):
            halfSizes(halfSizes),
            centerOfMass(centerOfMass),
            orientation(orientation) {
        axis[0] = orientation.rotateVector(Vector3<T>(halfSizes[0], 0.0, 0.0));
        axis[1] = orientation.rotateVector(Vector3<T>(0.0, halfSizes[1], 0.0));
        axis[2] = orientation.rotateVector(Vector3<T>(0.0, 0.0, halfSizes[2]));

        normalizedAxis[0] = axis[0].normalize();
        normalizedAxis[1] = axis[1].normalize();
        normalizedAxis[2] = axis[2].normalize();
    }

    template<class T> OBBox<T>::OBBox(const AABBox<T>& aabb) :
            halfSizes(Vector3<T>((aabb.getMax().X - aabb.getMin().X) / (T)2.0, (aabb.getMax().Y - aabb.getMin().Y) / (T)2.0, (aabb.getMax().Z - aabb.getMin().Z) / (T)2.0)),
            centerOfMass((aabb.getMin() + aabb.getMax()) / (T)2.0) {
        axis[0] = Vector3<T>(halfSizes[0], 0.0, 0.0);
        axis[1] = Vector3<T>(0.0, halfSizes[1], 0.0);
        axis[2] = Vector3<T>(0.0, 0.0, halfSizes[2]);

        normalizedAxis[0] = Vector3<T>(1.0, 0.0, 0.0);
        normalizedAxis[1] = Vector3<T>(0.0, 1.0, 0.0);
        normalizedAxis[2] = Vector3<T>(0.0, 0.0, 1.0);
    }

    template<class T> OBBox<T>::OBBox(const Sphere<T>& sphere) :
            halfSizes(Vector3<T>(sphere.getRadius(), sphere.getRadius(), sphere.getRadius())),
            centerOfMass(sphere.getCenterOfMass()) {
        axis[0] = Vector3<T>(halfSizes[0], 0.0, 0.0);
        axis[1] = Vector3<T>(0.0, halfSizes[1], 0.0);
        axis[2] = Vector3<T>(0.0, 0.0, halfSizes[2]);

        normalizedAxis[0] = Vector3<T>(1.0, 0.0, 0.0);
        normalizedAxis[1] = Vector3<T>(0.0, 1.0, 0.0);
        normalizedAxis[2] = Vector3<T>(0.0, 0.0, 1.0);
    }

    template<class T> T OBBox<T>::getHalfSize(unsigned int index) const {
        return halfSizes[index];
    }

    template<class T> const Vector3<T>& OBBox<T>::getHalfSizes() const {
        return halfSizes;
    }

    template<class T> const Point3<T>& OBBox<T>::getCenterOfMass() const {
        return centerOfMass;
    }

    template<class T> const Quaternion<T> &OBBox<T>::getOrientation() const {
        return orientation;
    }

    template<class T> const Vector3<T>& OBBox<T>::getAxis(unsigned int index) const {
        return axis[index];
    }

    template<class T> const Vector3<T>& OBBox<T>::getNormalizedAxis(unsigned int index) const {
        return normalizedAxis[index];
    }

    /**
     * Points of OBBox sorted first on positive X axis, then on positive Y axis and then on positive Z axis.
     */
    template<class T> std::vector<Point3<T>> OBBox<T>::getPoints() const {
        std::vector<Point3<T>> points;
        points.reserve(8);

        for (unsigned int i = 0; i < 8; ++i) {
            points.push_back(getPoint(i));
        }

        return points;
    }

    /**
     * Points of OBBox sorted first on positive X axis, then on positive Y axis and then on positive Z axis.
     */
    template<class T> Point3<T> OBBox<T>::getPoint(unsigned int index) const {
        switch(index) {
            case 0:
                return centerOfMass.translate(axis[0] + axis[1] + axis[2]);
            case 1:
                return centerOfMass.translate(axis[0] + axis[1] - axis[2]);
            case 2:
                return centerOfMass.translate(axis[0] - axis[1] + axis[2]);
            case 3:
                return centerOfMass.translate(axis[0] - axis[1] - axis[2]);
            case 4:
                return centerOfMass.translate(-axis[0] + axis[1] + axis[2]);
            case 5:
                return centerOfMass.translate(-axis[0] + axis[1] - axis[2]);
            case 6:
                return centerOfMass.translate(-axis[0] - axis[1] + axis[2]);
            case 7:
                return centerOfMass.translate(-axis[0] - axis[1] - axis[2]);
            default:
                break;
        }

        throw std::invalid_argument("Invalid index: " + std::to_string(index));
    }

    template<class T> Point3<T> OBBox<T>::getSupportPoint(const Vector3<T>& direction) const {
        Point3<T> maxPoint = getPoint(0);
        T maxPointDotDirection = maxPoint.toVector().dotProduct(direction);

        for (unsigned int i = 1; i < 8; ++i) {
            Point3<T> currentPoint = getPoint(i);
            T currentPointDotDirection  = currentPoint.toVector().dotProduct(direction);
            if (currentPointDotDirection > maxPointDotDirection) {
                maxPointDotDirection = currentPointDotDirection;
                maxPoint = currentPoint;
            }
        }

        return maxPoint;
    }

    template<class T> AABBox<T> OBBox<T>::toAABBox() const {
        Point3<T> min(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
        Point3<T> max(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max(), -std::numeric_limits<T>::max());

        for (unsigned int i = 0; i < 8; ++i) {
            Point3<T> point = getPoint(i);
            for (unsigned int coordinate = 0; coordinate < 3; ++coordinate) {
                if (point[coordinate] < min[coordinate]) {
                    min[coordinate] = point[coordinate];
                }

                if (point[coordinate] > max[coordinate]) {
                    max[coordinate] = point[coordinate];
                }
            }
        }

        return AABBox<T>(min, max);
    }

    /**
    * @return True if the bounding box collides or is inside this bounding box
    */
    template<class T> bool OBBox<T>::collideWithOBBox(const OBBox<T>& bbox) const {
        //rough collision test
        T minHalfSize = std::min(std::min(halfSizes[0], halfSizes[1]), halfSizes[2]);
        T sumMinRadius = minHalfSize + std::min(std::min(bbox.getHalfSize(0), bbox.getHalfSize(1)), bbox.getHalfSize(2));
        if (centerOfMass.squareDistance(bbox.getCenterOfMass()) < (sumMinRadius * sumMinRadius)) {
            return true;
        }

        return separatedAxisTheoremCollision(bbox);
    }

    /**
    * @return True if the bounding box collides or is inside this bounding box
    */
    template<class T> bool OBBox<T>::collideWithAABBox(const AABBox<T>& bbox) const {
        //rough collision test
        T minHalfSize = std::min(std::min(halfSizes[0], halfSizes[1]), halfSizes[2]);
        T sumMinRadius = minHalfSize + bbox.getMinHalfSize();
        if (centerOfMass.squareDistance(bbox.getCenterOfMass()) < (sumMinRadius * sumMinRadius)) {
            return true;
        }

        return separatedAxisTheoremCollision(OBBox<T>(bbox));
    }

    template<class T> bool OBBox<T>::separatedAxisTheoremCollision(const OBBox<T>& bbox) const {
        //separated axis theorem (see http://jkh.me/files/tutorials/Separating%20Axis%20Theorem%20for%20Oriented%20Bounding%20Boxes.pdf)
        Vector3<T> distCenterPoint = getCenterOfMass().vector(bbox.getCenterOfMass());
        const T AdotB[3][3] = {
                {normalizedAxis[0].dotProduct(bbox.getNormalizedAxis(0)), normalizedAxis[0].dotProduct(bbox.getNormalizedAxis(1)), normalizedAxis[0].dotProduct(bbox.getNormalizedAxis(2))},
                {normalizedAxis[1].dotProduct(bbox.getNormalizedAxis(0)), normalizedAxis[1].dotProduct(bbox.getNormalizedAxis(1)), normalizedAxis[1].dotProduct(bbox.getNormalizedAxis(2))},
                {normalizedAxis[2].dotProduct(bbox.getNormalizedAxis(0)), normalizedAxis[2].dotProduct(bbox.getNormalizedAxis(1)), normalizedAxis[2].dotProduct(bbox.getNormalizedAxis(2))}
        };
        constexpr T EPSILON = (T)0.00001; //projectionAxis could be near to zero: need epsilon to avoid rounding error

        //case 1, 2, 3 (projectionAxis = axis[0] | axis[1] | axis[2])
        for (unsigned int i = 0; i < 3; i++) {
            if (std::abs(distCenterPoint.dotProduct(normalizedAxis[i]))
                    > this->getHalfSize(i)
                    + std::abs(bbox.getHalfSize(0) * AdotB[i][0])
                    + std::abs(bbox.getHalfSize(1) * AdotB[i][1])
                    + std::abs(bbox.getHalfSize(2) * AdotB[i][2])) {
                return false;
            }
        }

        //case 4, 5, 6 (projectionAxis = bbox.getAxis(0) | bbox.getAxis(1) | bbox.getAxis(2))
        for (unsigned int i = 0; i < 3; i++) {
            if (std::abs(distCenterPoint.dotProduct(bbox.getNormalizedAxis(i)))
                    > std::abs(this->getHalfSize(0) * AdotB[0][i])
                    + std::abs(this->getHalfSize(1) * AdotB[1][i])
                    + std::abs(this->getHalfSize(2) * AdotB[2][i])
                    + bbox.getHalfSize(i)) {
                return false;
            }
        }

        //case 7 (projectionAxis = axis[0].crossProduct(bbox.getAxis(0)))
        if (std::abs(distCenterPoint.dotProduct(normalizedAxis[0].crossProduct(bbox.getNormalizedAxis(0)))) - EPSILON
                > std::abs(this->getHalfSize(1) * AdotB[2][0])
                + std::abs(this->getHalfSize(2) * AdotB[1][0])
                + std::abs(bbox.getHalfSize(1) * AdotB[0][2])
                + std::abs(bbox.getHalfSize(2) * AdotB[0][1])) {
            return false;
        }

        //case 8 (projectionAxis = axis[0].crossProduct(bbox.getAxis(1)))
        if (std::abs(distCenterPoint.dotProduct(normalizedAxis[0].crossProduct(bbox.getNormalizedAxis(1)))) - EPSILON
                > std::abs(this->getHalfSize(1) * AdotB[2][1])
                + std::abs(this->getHalfSize(2) * AdotB[1][1])
                + std::abs(bbox.getHalfSize(0) * AdotB[0][2])
                + std::abs(bbox.getHalfSize(2) * AdotB[0][0])) {
            return false;
        }

        //case 9 (projectionAxis = axis[0].crossProduct(bbox.getAxis(2)))
        if (std::abs(distCenterPoint.dotProduct(normalizedAxis[0].crossProduct(bbox.getNormalizedAxis(2)))) - EPSILON
                > std::abs(this->getHalfSize(1) * AdotB[2][2])
                + std::abs(this->getHalfSize(2) * AdotB[1][2])
                + std::abs(bbox.getHalfSize(0) * AdotB[0][1])
                + std::abs(bbox.getHalfSize(1) * AdotB[0][0])) {
            return false;
        }

        //case 10 (projectionAxis = axis[1].crossProduct(bbox.getAxis(0)))
        if (std::abs(distCenterPoint.dotProduct(normalizedAxis[1].crossProduct(bbox.getNormalizedAxis(0)))) - EPSILON
                > std::abs(this->getHalfSize(0) * AdotB[2][0])
                + std::abs(this->getHalfSize(2) * AdotB[0][0])
                + std::abs(bbox.getHalfSize(1) * AdotB[1][2])
                + std::abs(bbox.getHalfSize(2) * AdotB[1][1])) {
            return false;
        }

        //case 11 (projectionAxis = axis[1].crossProduct(bbox.getAxis(1)))
        if (std::abs(distCenterPoint.dotProduct(normalizedAxis[1].crossProduct(bbox.getNormalizedAxis(1)))) - EPSILON
                > std::abs(this->getHalfSize(0) * AdotB[2][1])
                + std::abs(this->getHalfSize(2) * AdotB[0][1])
                + std::abs(bbox.getHalfSize(0) * AdotB[1][2])
                + std::abs(bbox.getHalfSize(2) * AdotB[1][0])) {
            return false;
        }

        //case 12 (projectionAxis = axis[1].crossProduct(bbox.getAxis(2)))
        if (std::abs(distCenterPoint.dotProduct(normalizedAxis[1].crossProduct(bbox.getNormalizedAxis(2)))) - EPSILON
                > std::abs(this->getHalfSize(0) * AdotB[2][2])
                + std::abs(this->getHalfSize(2) * AdotB[0][2])
                + std::abs(bbox.getHalfSize(0) * AdotB[1][1])
                + std::abs(bbox.getHalfSize(1) * AdotB[1][0])) {
            return false;
        }

        //case 13 (projectionAxis = axis[2].crossProduct(bbox.getAxis(0)))
        if (std::abs(distCenterPoint.dotProduct(normalizedAxis[2].crossProduct(bbox.getNormalizedAxis(0)))) - EPSILON
                > std::abs(this->getHalfSize(0) * AdotB[1][0])
                + std::abs(this->getHalfSize(1) * AdotB[0][0])
                + std::abs(bbox.getHalfSize(1) * AdotB[2][2])
                + std::abs(bbox.getHalfSize(2) * AdotB[2][1])) {
            return false;
        }

        //case 14 (projectionAxis = axis[2].crossProduct(bbox.getAxis(1)))
        if (std::abs(distCenterPoint.dotProduct(normalizedAxis[2].crossProduct(bbox.getNormalizedAxis(1)))) - EPSILON
                > std::abs(this->getHalfSize(0) * AdotB[1][1])
                + std::abs(this->getHalfSize(1) * AdotB[0][1])
                + std::abs(bbox.getHalfSize(0) * AdotB[2][2])
                + std::abs(bbox.getHalfSize(2) * AdotB[2][0])) {
            return false;
        }

        //case 15 (projectionAxis = axis[2].crossProduct(bbox.getAxis(2)))
        return std::abs(distCenterPoint.dotProduct(normalizedAxis[2].crossProduct(bbox.getNormalizedAxis(2)))) - EPSILON
                <= std::abs(this->getHalfSize(0) * AdotB[1][2])
                + std::abs(this->getHalfSize(1) * AdotB[0][2])
                + std::abs(bbox.getHalfSize(0) * AdotB[2][1])
                + std::abs(bbox.getHalfSize(1) * AdotB[2][0]);
    }

    template<class T> OBBox<T> operator *(const Matrix4<T>& m, const OBBox<T>& obb) {
        #ifndef NDEBUG
            //projection matrix not accepted because result will not an oriented bounding box
            assert(fabs(m(3,0)) < std::numeric_limits<T>::epsilon());
            assert(fabs(m(3,1)) < std::numeric_limits<T>::epsilon());
            assert(fabs(m(3,2)) < std::numeric_limits<T>::epsilon());
        #endif

        //build OBB
        Matrix3<T> m3 = m.toMatrix3();
        Vector3<T> halfSizes(
                (m3 * obb.getAxis(0)).length(),
                (m3 * obb.getAxis(1)).length(),
                (m3 * obb.getAxis(2)).length()
        );
        Point3<T> centerOfMass = (m * Point4<T>(obb.getCenterOfMass())).toPoint3();

        return OBBox<T>(halfSizes, centerOfMass, Quaternion<T>::fromRotationMatrix(m3));
    }

    template<class T> OBBox<T> operator *(const OBBox<T>& obb, const Matrix4<T>& m) {
        return m * obb;
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const OBBox<T>& obbox) {
        stream.setf(std::ios::left);
        stream << "OBBox point 1: " << obbox.getPoint(0) << std::endl;
        stream << "OBBox point 2: " << obbox.getPoint(1) << std::endl;
        stream << "OBBox point 3: " << obbox.getPoint(2) << std::endl;
        stream << "OBBox point 4: " << obbox.getPoint(3) << std::endl;
        stream << "OBBox point 5: " << obbox.getPoint(4) << std::endl;
        stream << "OBBox point 6: " << obbox.getPoint(5) << std::endl;
        stream << "OBBox point 7: " << obbox.getPoint(6) << std::endl;
        stream << "OBBox point 8: " << obbox.getPoint(7);

        return stream;
    }

    //explicit template
    template class OBBox<float>;
    template OBBox<float> operator *<float>(const Matrix4<float>&, const OBBox<float>&);
    template OBBox<float> operator *<float>(const OBBox<float>&, const Matrix4<float>&);
    template std::ostream& operator <<<float>(std::ostream&, const OBBox<float>&);

    template class OBBox<double>;
    template OBBox<double> operator *<double>(const Matrix4<double>&, const OBBox<double>&);
    template OBBox<double> operator *<double>(const OBBox<double>&, const Matrix4<double>&);
    template std::ostream& operator <<<double>(std::ostream&, const OBBox<double>&);

}
