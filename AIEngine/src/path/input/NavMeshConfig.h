#ifndef ENGINE_NAVMESHCONFIG_H
#define ENGINE_NAVMESHCONFIG_H

namespace urchin
{

	class NavMeshConfig
	{
		public:
			NavMeshConfig(float, float);

		private:
			float agentHeight;
			float agentRadius;
	};

}

#endif
