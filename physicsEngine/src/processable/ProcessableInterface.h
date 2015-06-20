#ifndef ENGINE_PROCESSABLEINTERFACE_H
#define ENGINE_PROCESSABLEINTERFACE_H

#include "UrchinCommon.h"

namespace urchin
{

	class PhysicsWorld;

	/**
	* A processable will be processed in process loop of a physics world. Note that process execution is
	* done in the physics thread: make sure the concrete class is thread safe.
	*/
	class ProcessableInterface
	{
		public:
			ProcessableInterface();
			virtual ~ProcessableInterface();

			virtual void initialize(PhysicsWorld *) = 0;

			virtual void setup(float, const Vector3<float> &) = 0;
			virtual void process(float, const Vector3<float> &) = 0;
	};

}

#endif
