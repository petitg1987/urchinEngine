#ifndef URCHINENGINE_SCENEOBJECT_H
#define URCHINENGINE_SCENEOBJECT_H

#include <string>
#include "Urchin3dEngine.h"
#include "UrchinPhysicsEngine.h"
#include "UrchinAIEngine.h"

#include "resources/common/SceneEntity.h"

namespace urchin
{

	/**
	* Represent an object on the scene (3d model and physics)
	*/
	class SceneObject : public SceneEntity
	{
		//XML tags
		#define MODEL_TAG "model"
		#define PHYSICS_TAG "physics"

		//XML attributes
		#define NAME_ATTR "name"

		public:
			friend class Map;

			SceneObject();
			~SceneObject();

			void setObjectManagers(Renderer3d *, PhysicsWorld *, AIManager *);

			const std::string &getName() const;
			void setName(const std::string &);

			Model *getModel() const;
			void setModel(Model *);

			RigidBody *getRigidBody() const override;
			void moveTo(const Transform<float> &) override;

			void setupInteractiveBody(RigidBody *);

		private:
			void loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
			void writeOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;

			void setupRigidBody(RigidBody *);
			void setupAIObject(RigidBody *);

			void deleteRigidBody();
			void deleteAIObjects();

			Renderer3d *renderer3d;
			PhysicsWorld *physicsWorld;
			AIManager *aiManager;

			std::string name;
			Model *model;
			RigidBody *rigidBody;
			std::shared_ptr<AIObject> aiObject;
	};

}

#endif
