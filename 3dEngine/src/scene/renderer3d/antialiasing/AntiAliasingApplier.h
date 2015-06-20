#ifndef ENGINE_ANTIALIASINGAPPLIER_H
#define ENGINE_ANTIALIASINGAPPLIER_H

#include "utils/shader/ShaderManager.h"

namespace urchin
{

	class AntiAliasingApplier
	{
		public:
			AntiAliasingApplier();
			~AntiAliasingApplier();

			void initialize();
			void onResize(int, int);

			void applyOn(unsigned int);
		private:
			bool isInitialized;

			unsigned int fxaaShader;
			int texLoc, invSceneSizeLoc;

			unsigned int *bufferIDs;
			unsigned int vertexArrayObject;
			enum //buffer IDs indexes
			{
				VAO_VERTEX_POSITION = 0,
				VAO_TEX_COORD
			};
			enum //shader input
			{
				SHADER_VERTEX_POSITION = 0,
				SHADER_TEX_COORD
			};
	};

}

#endif
