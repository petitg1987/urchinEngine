#ifndef ENGINE_RIGIDBODYREADERWRITER_H
#define ENGINE_RIGIDBODYREADERWRITER_H

#include <memory>
#include <string>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

namespace urchin
{

	class RigidBodyReaderWriter
	{
		//XML tags
		#define SHAPE_TAG "shape"
		#define MASS_TAG "mass"
		#define RESTITUTION_TAG "restitution"
		#define FRICTION_TAG "friction"
		#define ROLLING_FRICTION_TAG "rollingFriction"
		#define LINEAR_DAMPING_TAG "linearDamping"
		#define ANGULAR_DAMPING_TAG "angularDamping"
		#define LINEAR_FACTOR_TAG "linearFactor"
		#define ANGULAR_FACTOR_TAG "angularFactor"

		public:
			RigidBodyReaderWriter();
			~RigidBodyReaderWriter();

			RigidBody *loadFrom(std::shared_ptr<XmlChunk>, const std::string &, const Transform<float> &, const XmlParser &) const;
			void writeOn(std::shared_ptr<XmlChunk>, const RigidBody *, XmlWriter &) const;

		private:
			void loadBodyPropertiesOn(RigidBody *, std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writeBodyPropertiesOn(std::shared_ptr<XmlChunk>, const RigidBody *, XmlWriter &) const;
	};

}

#endif
