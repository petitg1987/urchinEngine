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
		this->selectedCompoundShapeComponent = selectedCompoundShapeComponent;
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
				const CollisionCompoundShape *compoundShape = static_cast<const CollisionCompoundShape *>(bodyShape.get());
				const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &localizedShapes = compoundShape->getLocalizedShapes();
				for(unsigned int i=0; i<localizedShapes.size(); ++i)
				{
					std::shared_ptr<const LocalizedCollisionShape> localizedShape = localizedShapes[i];

					PhysicsTransform transform = PhysicsTransform(modelTransform.getPosition(), modelTransform.getOrientation()) * localizedShape->transform;
					std::shared_ptr<CollisionConvexObject3D> bodyObject = localizedShape->shape->toConvexObject(transform);

					GeometryModel *geometryModel = retrieveSingleGeometry(localizedShape->shape->getShapeType(), bodyObject.get());

					if(selectedCompoundShapeComponent.get()!=nullptr && selectedCompoundShapeComponent->position==localizedShape->position)
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

			for(unsigned int i=0; i<bodyShapeModels.size(); ++i)
			{
				sceneManager->getActiveRenderer3d()->addGeometry(bodyShapeModels[i]);
			}
		}
	}

	GeometryModel *BodyShapeDisplayer::retrieveSingleGeometry(CollisionShape3D::ShapeType shapeType, const CollisionConvexObject3D *bodyObject)
	{
		if(shapeType==CollisionShape3D::SPHERE_SHAPE)
		{
			return retrieveSphereGeometry(bodyObject);
		}else if(shapeType==CollisionShape3D::BOX_SHAPE)
		{
			return retrieveBoxGeometry(bodyObject);
		}else if(shapeType==CollisionShape3D::CYLINDER_SHAPE)
		{
			return retrieveCylinderGeometry(bodyObject);
		}else if(shapeType==CollisionShape3D::CONE_SHAPE)
		{
			return retrieveConeGeometry(bodyObject);
		}else if(shapeType==CollisionShape3D::CAPSULE_SHAPE)
		{
			return retrieveCapsuleGeometry(bodyObject);
		}else if(shapeType==CollisionShape3D::CONVEX_HULL_SHAPE)
		{
			return retrieveConvexHullGeometry(bodyObject);
		}else if(shapeType==CollisionShape3D::COMPOUND_SHAPE)
		{
			throw std::invalid_argument("Impossible to return a simple geometry from a compound shape");
		}

		throw std::invalid_argument("Unknown shape type to retrieve geometry: " + shapeType);
	}

	GeometryModel *BodyShapeDisplayer::retrieveSphereGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const CollisionSphereObject *sphereObject = static_cast<const CollisionSphereObject *>(bodyObject);
		return new SphereModel(sphereObject->retrieveSphere(), 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveBoxGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const CollisionBoxObject *boxObject = static_cast<const CollisionBoxObject *>(bodyObject);
		return new OBBoxModel(boxObject->retrieveOBBox());
	}

	GeometryModel *BodyShapeDisplayer::retrieveCylinderGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const CollisionCylinderObject *cylinderObject = static_cast<const CollisionCylinderObject *>(bodyObject);
		return new CylinderModel(cylinderObject->retrieveCylinder(), 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveConeGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const CollisionConeObject *coneObject = static_cast<const CollisionConeObject *>(bodyObject);
		return new ConeModel(coneObject->retrieveCone(), 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveCapsuleGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const CollisionCapsuleObject *capsuleObject = static_cast<const CollisionCapsuleObject *>(bodyObject);
		return new CapsuleModel(capsuleObject->retrieveCapsule(), 15, 15);
	}

	GeometryModel *BodyShapeDisplayer::retrieveConvexHullGeometry(const CollisionConvexObject3D *bodyObject) const
	{
		const CollisionConvexHullObject *convexHullObject = static_cast<const CollisionConvexHullObject *>(bodyObject);
		return new PointsModel(convexHullObject->retrieveConvexHull().getPoints(), 5.0);
	}

	void BodyShapeDisplayer::cleanCurrentDisplay()
	{
		for(unsigned int i=0; i<bodyShapeModels.size(); ++i)
		{
			sceneManager->getActiveRenderer3d()->removeGeometry(bodyShapeModels[i]);
			delete bodyShapeModels[i];
		}

		bodyShapeModels.clear();
	}
}
