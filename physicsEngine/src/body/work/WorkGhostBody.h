#ifndef ENGINE_WORKGHOSTBODY_H
#define ENGINE_WORKGHOSTBODY_H

#include <string>
#include "UrchinCommon.h"

#include "collision/broadphase/PairContainer.h"
#include "body/work/AbstractWorkBody.h"

namespace urchin
{

	/**
	* A ghost body don't act with the physics world but it's able to known which bodies are in collision with it.
	*/
	class WorkGhostBody : public AbstractWorkBody
	{
		public:
			WorkGhostBody(const std::string &, const PhysicsTransform &, const std::shared_ptr<const CollisionShape3D> &);
			~WorkGhostBody();

			static WorkGhostBody *upCast(AbstractWorkBody *);
			static const WorkGhostBody *upCast(const AbstractWorkBody *);

			PairContainer *getPairContainer() const;

			bool isGhostBody() const;

		private:
			PairContainer *pairContainer;

	};

}

#endif
