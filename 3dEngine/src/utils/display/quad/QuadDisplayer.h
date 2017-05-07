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

			void display() const;

		private:
			void initializeDisplay();

			unsigned int numberOfQuad;
			unsigned int dimension; //2=2D, 3=3D

			unsigned int bufferUsage;

			unsigned int vertexDataType;
			void *vertexCoord;

			unsigned int textureDataType;
			void *textureCoord;

			unsigned int bufferIDs[2], vertexArrayObject;
			enum //buffer IDs indices
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

