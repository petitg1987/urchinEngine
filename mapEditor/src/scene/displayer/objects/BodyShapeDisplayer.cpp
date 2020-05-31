#include <memory>
#include <stdexcept>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "BodyShapeDisplayer.h"

namespace urchin
{

	BodyShapeDisplayer::BodyShapeDisplayer(SceneManager *sceneManager) :
		sceneManager(sceneManager),
		selectedSceneObject(nullptr)
	{

	}

	BodyShapeDisplayer::~BodyShapeDisplayer()
	{
		cleanCurrentDisplay();
	}

	void BodyShapeDisplayer::setSelectedSceneObject(const SceneObject *selectedSceneObject)
	{
		this->selectedSceneObject = selectedSceneObject;
	}

	void BodyShapeDisplayer::setSelectedCompoundShapeComponent(std::shared_ptr<const LocalizedCollisionShape> selectedCompoundShapeComponent)
	{
		this->selectedCompoundShapeComponent = std::move(selectedCompoundShapeComponent);
	}

	void BodyShapeDisplayer::displayBodyShape()
	{
		cleanCurrentDisplay();

		if(selectedSceneObject && selectedSceneObject->getRigidBody())
		{
			const Transform<float> &modelTransform = selectedSceneObject->getModel()->getTransform();
			std::shared_ptr<const CollisionShape3D> bodyShape = selectedSceneObject->getRigidBody()->getScaledShape();

			if(bodyShape->isConcave())
			{
				PhysicsTransform transform(modelTransform.getPosition(), modelTransform.getOrientation());
				AABBox<float> heightfieldAABBox = bodyShape->toAABBox(transform);

				GeometryModel *geometryModel = new AABBoxModel(heightfieldAABBox);
				geometryModel->setColor(0.0, 1.0, 0.0);
				bodyShapeModels.push_back(geometryModel);
			} else if(bodyShape->isCompound())
			{
				const auto *compoundShape = dynamic_cast<const CollisionCompoundShape *>(bodyShape.get());
				const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &localizedShapes = compoundShape->getLocalizedShapes();
				for (const auto &localizedShape : localizedShapes)
				{
					PhysicsTransform transform = PhysicsTransform(modelTransform.getPosition(), modelTransform.getOrientation()) * localizedShape->transform;
					std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> bodyObject = localizedShape->shape->toConvexObject(transform);

					GeometryModel *geometryModel = retrieveSingleGeometry(localizedShape->shape->getShapeType(), bodyObject);

					if (selectedCompoundShapeComponent != nullptr && selectedCompoundShapeComponent->position == localizedShape->position)
					{
						geometryModel->setColor(1.0, 1.0, 1.0);
					} else
					{
						geometryModel->setColor(0.0, 1.0, 0.0);
					}

					bodyShapeModels.push_back(geometryModel);
				}
			} else if(bodyShape->isConvex())
			{
				PhysicsTransform transform(modelTransform.getPosition(), modelTransform.getOrientation());
				std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> bodyObject = bodyShape->toConvexObject(transform);

				GeometryModel *geometryModel = retrieveSingleGeometry(bodyShape->getShapeType(), bodyObject);
				geometryModel->setColor(0.0, 1.0, 0.0);
				bodyShapeModels.push_back(geometryModel);
			} else
			{
				throw std::invalid_argument("Unknown shape type category: " + std::to_string(bodyShape->getShapeType()));
			}

			for (auto &bodyShapeModel : bodyShapeModels)
			{
				sceneManager->getActiveRenderer3d()->getGeometryManager()->addGeometry(bodyShapeModel);
			}
		}
	}

	GeometryModel *BodyShapeDisplayer::retrieveSingleGeometry(CollisionShape3D::ShapeType shapeType, const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> &bodyObject)
	{
		if(shapeType==CollisionShape3D::SPHERE_SHAPE)
		{
			return retrieveSphereGeometry(bodyObject);
		}
		if(shapeType==CollisionShape3D::BOX_SHAPE)
		{
			return retrieveBoxGeometry(bodyObject);
		}
		if(shapeType==CollisionShape3D::CYLINDER_SHAPE)
		{
			return retrieveCylinderGeometry(bodyObject);
		}
		if(shapeType==CollisionShape3D::CONE_SHAPE)
		{
			return retrieveConeGeometry(bodyObject);
		}
		if(shapeType==CollisionShape3D::CAPSULE_SHAPE)
		{
			return retrieveCapsuleGeometry(bodyObject);
		}
		if(shapeType==CollisionShape3D::CONVEX_HULL_SHAPE)
		{
			return retrieveConvexHullGeometry(bodyObject);
		}
		if(shapeType==CollisionShape3D::COMPOUND_SHAPE)
		{
			throw std::invalid_argument("Impossible to return a simple geometry from a compound shape");
		}

		throw std::invalid_argument("Unknown shape type to retrieve geometry: " + std::to_string(shapeType));
	}

	GeometryModel *BodyShapeDisplayer::retrieveSphereGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> &bodyObject) const
	{
		const auto *sphereObject = dynamic_cast<const CollisionSphereObject *>(bodyObject.get());
		return new SphereModel(sphereObject->retrieveSphere(), 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveBoxGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> &bodyObject) const
	{
		const auto *boxObject = dynamic_cast<const CollisionBoxObject *>(bodyObject.get());
		return new OBBoxModel(boxObject->retrieveOBBox());
	}

	GeometryModel *BodyShapeDisplayer::retrieveCylinderGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> &bodyObject) const
	{
		const auto *cylinderObject = dynamic_cast<const CollisionCylinderObject *>(bodyObject.get());
		return new CylinderModel(cylinderObject->retrieveCylinder(), 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveConeGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> &bodyObject) const
	{
		const auto *coneObject = dynamic_cast<const CollisionConeObject *>(bodyObject.get());
		return new ConeModel(coneObject->retrieveCone(), 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveCapsuleGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> &bodyObject) const
	{
		const auto *capsuleObject = dynamic_cast<const CollisionCapsuleObject *>(bodyObject.get());
		return new CapsuleModel(capsuleObject->retrieveCapsule(), 15, 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveConvexHullGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> &bodyObject) const
	{
		const auto *convexHullObject = dynamic_cast<const CollisionConvexHullObject *>(bodyObject.get());
		return new PointsModel(convexHullObject->getPointsWithMargin(), 5.0);
	}

	void BodyShapeDisplayer::cleanCurrentDisplay()
	{
		for (auto &bodyShapeModel : bodyShapeModels)
		{
			sceneManager->getActiveRenderer3d()->getGeometryManager()->removeGeometry(bodyShapeModel);
			delete bodyShapeModel;
		}

		bodyShapeModels.clear();
	}
}
