#ifndef URCHINENGINE_OVERLAPPINGPAIR_H
#define URCHINENGINE_OVERLAPPINGPAIR_H

#include <memory>

#include "body/work/AbstractWorkBody.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"

namespace urchin
{

	/**
	* Overlapping pair representing two bodies potentially colliding
	*/
	class OverlappingPair
	{
		public:
			OverlappingPair(AbstractWorkBody *, AbstractWorkBody *);
			OverlappingPair(AbstractWorkBody *, AbstractWorkBody *, uint_fast64_t);
			OverlappingPair(const OverlappingPair &) = default;

			AbstractWorkBody *getBody(unsigned int) const;
			AbstractWorkBody *getBody1() const;
			AbstractWorkBody *getBody2() const;

			static uint_fast64_t computeBodiesId(const AbstractWorkBody *, const AbstractWorkBody *);
			uint_fast64_t getBodiesId() const;

			void setCollisionAlgorithm(std::shared_ptr<CollisionAlgorithm>);
			std::shared_ptr<CollisionAlgorithm> getCollisionAlgorithm() const;

		private:
			AbstractWorkBody *body1;
			AbstractWorkBody *body2;
			uint_fast64_t bodiesId;

			std::shared_ptr<CollisionAlgorithm> collisionAlgorithm;
	};

}

#endif
