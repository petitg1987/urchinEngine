#ifndef ENGINE_ANTIALIASINGAPPLIER_H
#define ENGINE_ANTIALIASINGAPPLIER_H

namespace urchin
{

	class AntiAliasingApplier
	{
		public:
			enum Quality
			{
				LOW = 0,
				MEDIUM,
				HIGH,
				VERY_HIGH
			};

			AntiAliasingApplier();
			~AntiAliasingApplier();

			void initialize();
			void onResize(int, int);

			void setQuality(Quality quality);

			void applyOn(unsigned int);
		private:
			void loadFxaaShader();

			bool isInitialized;

			//properties
			Quality quality;
			int sceneWidth, sceneHeight;

			//shader
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
