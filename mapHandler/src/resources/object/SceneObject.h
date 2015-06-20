#ifndef ENGINE_SCENEOBJECT_H
#define ENGINE_SCENEOBJECT_H

#include <string>

#include "Urchin3dEngine.h"
#include "UrchinPhysicsEngine.h"

namespace urchin
{

	/**
	* Represent an object on the scene (3d model and physics)
	*/
	class SceneObject
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

			void setObjectManagers(SceneManager *, PhysicsWorld *);

			std::string getName() const;
			void setName(const std::string &);

			Model *getModel() const;
			void setModel(Model *);

			RigidBody *getRigidBody() const;
			void setRigidBody(RigidBody *);

		private:
			void loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
			void writeOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;

			SceneManager *sceneManager;
			PhysicsWorld *physicsWorld;

			std::string name;
			Model *model;
			RigidBody *rigidBody;
	};

}

#endif
