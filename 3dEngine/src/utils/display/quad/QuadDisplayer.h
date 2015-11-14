#ifndef ENGINE_QUADDISPLAYER_H
#define ENGINE_QUADDISPLAYER_H

namespace urchin
{

	class QuadDisplayerBuilder;

	class QuadDisplayer
	{
		public:
			QuadDisplayer(const QuadDisplayerBuilder *);
			~QuadDisplayer();

			void initialize();

			void display() const;

		private:
			bool isInitialized;

			unsigned int bufferUsage;

			unsigned int vertexDataType;
			void *vertexCoord;

			unsigned int textureDataType;
			void *textureCoord;

			unsigned int bufferIDs[2], vertexArrayObject;
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

