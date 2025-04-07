#include <limits>
#include <cmath>
#include <stdexcept>
#include <functional>
#ifdef URCHIN_DEBUG
    #include <cassert>
#endif

#include <math/geometry/3d/object/OBBox.h>
#include <math/geometry/3d/Rectangle3D.h>
#include <math/algebra/point/Point4.h>
namespace urchin {

    template<class T> OBBox<T>::OBBox():
            ConvexObject3D<T>(ConvexObjectType::OBBOX),
            halfSizes(Vector3<T>(0.0, 0.0, 0.0)),
            centerOfMass(Point3<T>(0.0, 0.0, 0.0)) {
        axis[0] = Vector3<T>(0.0, 0.0, 0.0);
        axis[1] = Vector3<T>(0.0, 0.0, 0.0);
        axis[2] = Vector3<T>(0.0, 0.0, 0.0);

        normalizedAxis[0] = Vector3<T>(0.0, 0.0, 0.0);
        normalizedAxis[1] = Vector3<T>(0.0, 0.0, 0.0);
        normalizedAxis[2] = Vector3<T>(0.0, 0.0, 0.0);
    }

    template<class T> OBBox<T>::OBBox(const Vector3<T>& halfSizes, const Point3<T>& centerOfMass, const Quaternion<T>& orientation) :
            ConvexObject3D<T>(ConvexObjectType::OBBOX),
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
            ConvexObject3D<T>(ConvexObjectType::OBBOX),
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
            ConvexObject3D<T>(ConvexObjectType::OBBOX),
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
        switch (index) {
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

    /**
     * Vector from center to points of OBBox sorted first on positive X axis, then on positive Y axis and then on positive Z axis.
     */
    template<class T> Vector3<T> OBBox<T>::getVectorPoint(unsigned int index) const {
        switch (index) {
            case 0:
                return axis[0] + axis[1] + axis[2];
            case 1:
                return axis[0] + axis[1] - axis[2];
            case 2:
                return axis[0] - axis[1] + axis[2];
            case 3:
                return axis[0] - axis[1] - axis[2];
            case 4:
                return -axis[0] + axis[1] + axis[2];
            case 5:
                return -axis[0] + axis[1] - axis[2];
            case 6:
                return -axis[0] - axis[1] + axis[2];
            case 7:
                return -axis[0] - axis[1] - axis[2];
            default:
                break;
        }

        throw std::invalid_argument("Invalid index: " + std::to_string(index));
    }

    template<class T> Point3<T> OBBox<T>::getSupportPoint(const Vector3<T>& direction) const {
        Vector3<T> bestPointVector = getVectorPoint(0);
        T maxDotProduct = bestPointVector.dotProduct(direction);

        for (unsigned int i = 1; i < 8; ++i) {
            Vector3<T> currentPointVector = getVectorPoint(i);
            T currentDotProduct = currentPointVector.dotProduct(direction);
            if (currentDotProduct > maxDotProduct) {
                maxDotProduct = currentDotProduct;
                bestPointVector = currentPointVector;
            }
        }

        return centerOfMass.translate(bestPointVector);
    }

    /**
     * @return Orthogonal projection matrix based on OBBox
     * //TODO add comment like on AABBox
     */
    template<class T> Matrix4<T> OBBox<T>::toProjectionMatrix() const {
        // Create view matrix (world to OBB space)
        Matrix4<T> translate = Matrix4<T>::buildTranslation(-getCenterOfMass().X, -getCenterOfMass().Y, -getCenterOfMass().Z); //TODO create method accepting vector/point ?
        Matrix4<T> view = translate * Matrix4<T>(orientation.toMatrix3().transpose()); //TODO combine to avoid matrix multiplication + check order !

        //TODO create method in Matrix4 ?
        T left = -getHalfSize(0);
        T right = getHalfSize(0);
        T bottom = -getHalfSize(1);
        T top = getHalfSize(1);
        T near = -getHalfSize(2);
        T far = getHalfSize(2);

        T translationX = -((right + left) / (right - left)); //translation
        T translationY = (top + bottom) / (top - bottom);
        T translationZ = (T)0.5 - (T)0.5 * ((-near - far) / (-near + far));

        T scaleX = (T)2.0 / (right - left);
        T scaleY = (T)-2.0 / (top - bottom);
        T scaleZ = (T)-1.0 / (-near + far);

        Matrix4<T> orthographicProjection(
                scaleX, 0.0, 0.0, translationX,
                0.0, scaleY, 0.0, translationY,
                0.0, 0.0, scaleZ, translationZ,
                0.0, 0.0, 0.0, 1.0);

        // Create orthographic projection matrix for the OBB
        // glm::mat4 projection = glm::ortho(
        //     -obb.extents.x, obb.extents.x,
        //     -obb.extents.y, obb.extents.y,
        //     -obb.extents.z, obb.extents.z
        // );

        return orthographicProjection * view;
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
        T boxesSquareDistance = centerOfMass.squareDistance(bbox.getCenterOfMass());
        T minHalfSize = std::min(std::min(halfSizes[0], halfSizes[1]), halfSizes[2]);
        T sumMinRadius = minHalfSize + bbox.getMinHalfSize();
        if (boxesSquareDistance < (sumMinRadius * sumMinRadius)) {
            return true;
        }

        return separatedAxisTheoremCollision(bbox);
    }

    template<class T> Point3<T> OBBox<T>::intersectPoint(const Line3D<T>& line, bool& hasIntersection) const {
        Point3<T> bestIntersectionPoint(0.0f, 0.0f, 0.0f);
        hasIntersection = false;
        std::array<std::function<Rectangle3D<T>()>, 6> faceRectangles = {
                [this] { return Rectangle3D<T>({getPoint(4), getPoint(6), getPoint(7), getPoint(5)}); }, //left
                [this] { return Rectangle3D<T>({getPoint(0), getPoint(1), getPoint(3), getPoint(2)}); }, //right
                [this] { return Rectangle3D<T>({getPoint(5), getPoint(1), getPoint(0), getPoint(4)}); }, //top
                [this] { return Rectangle3D<T>({getPoint(3), getPoint(7), getPoint(6), getPoint(2)}); }, //bottom
                [this] { return Rectangle3D<T>({getPoint(4), getPoint(0), getPoint(2), getPoint(6)}); }, //front
                [this] { return Rectangle3D<T>({getPoint(1), getPoint(5), getPoint(7), getPoint(3)}); } //back
        };

        for (std::size_t i = 0; i < faceRectangles.size(); ++i) {
            bool hasRectangleIntersection;
            bool hasPlaneIntersection;
            Point3<T> intersectionPoint = faceRectangles[i]().intersectPoint(line, hasRectangleIntersection, hasPlaneIntersection);
            if (hasPlaneIntersection) {
                if (hasRectangleIntersection && (!hasIntersection || (line.getA().squareDistance(intersectionPoint) < line.getA().squareDistance(bestIntersectionPoint)))) {
                    bestIntersectionPoint = intersectionPoint;
                    hasIntersection = true;
                }
            } else if (i % 2 == 0) { //condition always true (when hasPlaneIntersection=false) except when float imprecision is bad (e.g.: collision detected on left plane but no on right plane)
                i++; //skip next parallel plane
            }
        }

        return bestIntersectionPoint;
    }

    template<class T> bool OBBox<T>::pointInsideSquare(const Point3<T>& testPoint, const std::array<Point3<T>, 4>& points, const Vector3<T>& normal) const {
        T orient1 = testPoint.vector(points[0]).crossProduct(testPoint.vector(points[1])).dotProduct(normal);
        T orient2 = testPoint.vector(points[1]).crossProduct(testPoint.vector(points[2])).dotProduct(normal);
        T orient3 = testPoint.vector(points[2]).crossProduct(testPoint.vector(points[3])).dotProduct(normal);
        T orient4 = testPoint.vector(points[3]).crossProduct(testPoint.vector(points[0])).dotProduct(normal);
        return (orient1 < 0.0 && orient2 < 0.0 && orient3 < 0.0 && orient4 < 0.0) || (orient1 > 0.0 && orient2 > 0.0 && orient3 > 0.0 && orient4 > 0.0);
    }

    template<class T> OBBox<T> operator *(const Matrix4<T>& m, const OBBox<T>& obb) {
        #ifdef URCHIN_DEBUG
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

        return OBBox<T>(halfSizes, centerOfMass, Quaternion<T>::fromRotationMatrix(m3) * obb.getOrientation());
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
