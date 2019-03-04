#ifndef URCHINENGINE_WORKGHOSTBODY_H
#define URCHINENGINE_WORKGHOSTBODY_H

#include <string>

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
			~WorkGhostBody() override;

			static WorkGhostBody *upCast(AbstractWorkBody *);
			static const WorkGhostBody *upCast(const AbstractWorkBody *);

			PairContainer *getPairContainer() const override;

			bool isGhostBody() const override;

		private:
			PairContainer *pairContainer;

	};

}

#endif
