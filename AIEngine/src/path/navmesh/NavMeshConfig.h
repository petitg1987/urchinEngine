#ifndef ENGINE_NAVMESHCONFIG_H
#define ENGINE_NAVMESHCONFIG_H

namespace urchin
{

	class NavMeshConfig
	{
		public:
			NavMeshConfig(float, float);

			void setMaxSlope(float);
			float getMaxSlope() const;

		private:
			float agentHeight;
			float agentRadius;

			float maxSlopeInRadian;
	};

}

#endif
