#ifndef ENGINE_INTEGRATETRANSFORMMANAGER_H
#define ENGINE_INTEGRATETRANSFORMMANAGER_H

#include "body/BodyManager.h"
#include "body/work/WorkRigidBody.h"

namespace urchin
{

	/**
	* Manager allowing to perform integration on bodies transformation
	*/
	class IntegrateTransformManager
	{
		public:
			IntegrateTransformManager(const BodyManager *);
			~IntegrateTransformManager();

			void integrateTransform(float);

		private:
			const BodyManager *bodyManager;
	};

}

#endif
