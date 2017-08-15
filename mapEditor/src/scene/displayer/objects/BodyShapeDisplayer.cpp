#include <memory>
#include <stdexcept>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "BodyShapeDisplayer.h"

namespace urchin
{

	BodyShapeDisplayer::BodyShapeDisplayer(SceneManager *sceneManager) :
		sceneManager(sceneManager)
	{

	}

	BodyShapeDisplayer::~BodyShapeDisplayer()
	{
		cleanCurrentDisplay();
	}

	void BodyShapeDisplayer::setSelectedCompoundShapeComponent(std::shared_ptr<const LocalizedCollisionShape> selectedCompoundShapeComponent)
	{
		this->selectedCompoundShapeComponent = std::move(selectedCompoundShapeComponent);
	}

	void BodyShapeDisplayer::displayBodyShapeFor(const SceneObject *sceneObject)
	{
		cleanCurrentDisplay();

		if(sceneObject!=nullptr)
		{
			const Transform<float> &modelTransform = sceneObject->getModel()->getTransform();
			std::shared_ptr<const CollisionShape3D> bodyShape = sceneObject->getRigidBody()->getScaledShape();

			if(bodyShape->getShapeType()==CollisionShape3D::COMPOUND_SHAPE)
			{
				const auto *compoundShape = dynamic_cast<const CollisionCompoundShape *>(bodyShape.get());
				const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &localizedShapes = compoundShape->getLocalizedShapes();
				for (auto localizedShape : localizedShapes)
				{
					PhysicsTransform transform = PhysicsTransform(modelTransform.getPosition(), modelTransform.getOrientation()) * localizedShape->transform;
					std::shared_ptr<CollisionConvexObject3D> bodyObject = localizedShape->shape->toConvexObject(transform);

					GeometryModel *geometryModel = retrieveSingleGeometry(localizedShape->shape->getShapeType(), bodyObject.get());

					if(selectedCompoundShapeComponent!=nullptr && selectedCompoundShapeComponent->position==localizedShape->position)
					{
						geometryModel->setColor(1.0, 1.0, 1.0);
					}else
					{
						geometryModel->setColor(0.0, 1.0, 0.0);
					}

					bodyShapeModels.push_back(geometryModel);
				}
			}else
			{
				PhysicsTransform transform(modelTransform.getPosition(), modelTransform.getOrientation());
				std::shared_ptr<CollisionConvexObject3D> bodyObject = bodyShape->toConvexObject(transform);

				GeometryModel *geometryModel = retrieveSingleGeometry(bodyShape->getShapeType(), bodyObject.get());
				geometryModel->setColor(0.0, 1.0, 0.0);
				bodyShapeModels.push_back(geometryModel);
			}

			for (auto &bodyShapeModel : bodyShapeModels)
			{
				sceneManager->getActiveRenderer3d()->addGeometry(bodyShapeModel);
			}
		}
	}

	GeometryModel *BodyShapeDisplayer::retrieveSingleGeometry(CollisionShape3D::ShapeType shapeType, const CollisionConvexObject3D *bodyObject)
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

	GeometryModel *BodyShapeDisplayer::retrieveSphereGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const auto *sphereObject = dynamic_cast<const CollisionSphereObject *>(bodyObject);
		return new SphereModel(sphereObject->retrieveSphere(), 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveBoxGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const auto *boxObject = dynamic_cast<const CollisionBoxObject *>(bodyObject);
		return new OBBoxModel(boxObject->retrieveOBBox());
	}

	GeometryModel *BodyShapeDisplayer::retrieveCylinderGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const auto *cylinderObject = dynamic_cast<const CollisionCylinderObject *>(bodyObject);
		return new CylinderModel(cylinderObject->retrieveCylinder(), 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveConeGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const auto *coneObject = dynamic_cast<const CollisionConeObject *>(bodyObject);
		return new ConeModel(coneObject->retrieveCone(), 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveCapsuleGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const auto *capsuleObject = dynamic_cast<const CollisionCapsuleObject *>(bodyObject);
		return new CapsuleModel(capsuleObject->retrieveCapsule(), 15, 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveConvexHullGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const auto *convexHullObject = dynamic_cast<const CollisionConvexHullObject *>(bodyObject);
		return new PointsModel(convexHullObject->getPointsWithMargin(), 5.0);
	}

	void BodyShapeDisplayer::cleanCurrentDisplay()
	{
		for (auto &bodyShapeModel : bodyShapeModels)
		{
			sceneManager->getActiveRenderer3d()->removeGeometry(bodyShapeModel);
			delete bodyShapeModel;
		}

		bodyShapeModels.clear();
	}
}
