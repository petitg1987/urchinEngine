#pragma once

#include <collision/GJKAlgorithm.h>
#include <collision/GJKResult.h>

#include <config/DebugCheck.h>
#include <config/ConfigService.h>
#include <config/FileSystem.h>

#include <io/file/PropertyFileHandler.h>
#include <io/file/FileReader.h>
#include <io/svg/SVGExporter.h>
#include <io/svg/shape/SVGPolygon.h>
#include <io/svg/shape/SVGLine.h>
#include <io/svg/shape/SVGCircle.h>
#include <io/map/MapSerializer.h>
#include <io/uda/UdaParser.h>
#include <io/uda/UdaAttribute.h>
#include <io/uda/UdaChunk.h>

#include <logger/Logger.h>
#include <logger/FileLogger.h>

#include <math/algebra/matrix/Matrix2.h>
#include <math/algebra/matrix/Matrix3.h>
#include <math/algebra/matrix/Matrix4.h>
#include <math/algebra/point/Point2.h>
#include <math/algebra/point/Point3.h>
#include <math/algebra/point/Point4.h>
#include <math/algebra/vector/Vector2.h>
#include <math/algebra/vector/Vector3.h>
#include <math/algebra/vector/Vector4.h>
#include <math/algebra/Quaternion.h>
#include <math/algebra/Transform.h>
#include <math/algebra/MathValue.h>
#include <math/algebra/Bezier.h>
#include <math/geometry/2d/Line2D.h>
#include <math/geometry/2d/IndexedTriangle2D.h>
#include <math/geometry/2d/shape/ConvexShape2D.h>
#include <math/geometry/2d/shape/RectangleShape.h>
#include <math/geometry/2d/shape/ConvexHullShape2D.h>
#include <math/geometry/2d/object/ConvexObject2D.h>
#include <math/geometry/2d/object/ConvexHull2D.h>
#include <math/geometry/2d/object/LineSegment2D.h>
#include <math/geometry/2d/object/Rectangle2D.h>
#include <math/geometry/2d/util/ResizePolygon2DService.h>
#include <math/geometry/3d/Line3D.h>
#include <math/geometry/3d/Ray.h>
#include <math/geometry/3d/Plane.h>
#include <math/geometry/3d/Rectangle3D.h>
#include <math/geometry/3d/IndexedTriangle3D.h>
#include <math/geometry/3d/shape/ConvexShape3D.h>
#include <math/geometry/3d/shape/BoxShape.h>
#include <math/geometry/3d/shape/SphereShape.h>
#include <math/geometry/3d/shape/TriangleShape3D.h>
#include <math/geometry/3d/shape/CapsuleShape.h>
#include <math/geometry/3d/shape/CylinderShape.h>
#include <math/geometry/3d/shape/ConeShape.h>
#include <math/geometry/3d/object/ConvexObject3D.h>
#include <math/geometry/3d/object/AABBox.h>
#include <math/geometry/3d/object/ConvexHull3D.h>
#include <math/geometry/3d/object/Frustum.h>
#include <math/geometry/3d/object/LineSegment3D.h>
#include <math/geometry/3d/object/OBBox.h>
#include <math/geometry/3d/object/Sphere.h>
#include <math/geometry/3d/object/Triangle3D.h>
#include <math/geometry/3d/object/Tetrahedron.h>
#include <math/geometry/3d/object/Capsule.h>
#include <math/geometry/3d/object/Cylinder.h>
#include <math/geometry/3d/object/Cone.h>
#include <math/geometry/3d/util/SortPointsService.h>
#include <math/geometry/3d/util/ResizeConvexHull3DService.h>
#include <math/geometry/3d/util/HeightfieldPointHelper.h>
#include <math/algorithm/MathFunction.h>
#include <math/algorithm/PascalTriangle.h>
#include <math/trigonometry/AngleConverter.h>

#include <partitioning/aabbtree/AABBTree.h>
#include <partitioning/aabbtree/AABBNode.h>
#include <partitioning/aabbtree/AABBNodeData.h>
#include <partitioning/octree/Octreeable.h>
#include <partitioning/octree/OctreeManager.h>
#include <partitioning/octree/Octree.h>
#include <partitioning/octree/filter/AcceptAllFilter.h>
#include <partitioning/octree/helper/OctreeableHelper.h>
#include <partitioning/grid/GridContainer.h>

#include <pattern/observer/Observable.h>
#include <pattern/observer/Observer.h>
#include <pattern/singleton/Singleton.h>
#include <pattern/singleton/ThreadSafeSingleton.h>
#include <pattern/singleton/SingletonContainer.h>
#include <pattern/singleton/SingletonInterface.h>
#include <pattern/tag/TaggableResource.h>
#include <pattern/tag/TagHolder.h>
#include <pattern/queue/CircularFifo.h>

#include <profiler/Profiler.h>
#include <profiler/ScopeProfiler.h>

#include <system/NumericalCheck.h>
#include <system/SignalHandler.h>
#include <system/SystemInfo.h>
#include <system/thread/LockById.h>
#include <system/thread/ScopeLockById.h>
#include <system/thread/SleepUtil.h>
#include <system/control/Control.h>

#include <util/DateTimeUtil.h>
#include <util/FileUtil.h>
#include <util/StringUtil.h>
#include <util/MapUtil.h>
#include <util/TypeConverter.h>
#include <util/UserAuthorityException.h>
#include <util/ArrayUtil.h>
#include <util/VectorUtil.h>
#include <util/HashUtil.h>
