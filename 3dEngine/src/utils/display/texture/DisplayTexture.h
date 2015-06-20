#ifndef ENGINE_DISPLAYTEXTURE_H
#define ENGINE_DISPLAYTEXTURE_H

#include "UrchinCommon.h"

namespace urchin
{

	/**
	* Display a frame buffer object
	* Sample of use:
	* 	DisplayTexture displayTexture(textureIDs[TEX_DEPTH], DisplayTexture::DEPTH_FACTOR);
	* 	displayTexture.setPosition(DisplayTexture::LEFT, DisplayTexture::BOTTOM);
	*	displayTexture.setFullscreen(false);
	*	displayTexture.display();
	*/
	class DisplayTexture
	{
		public:
			enum CoordinateX
			{
				LEFT,
				CENTER_X,
				RIGHT,
				USER_DEFINED_X
			};

			enum CoordinateY
			{
				TOP,
				CENTER_Y,
				BOTTOM,
				USER_DEFINED_Y
			};

			enum ColorFactor
			{
				DEFAULT_FACTOR,
				DEPTH_FACTOR
			};

			DisplayTexture(unsigned int, DisplayTexture::ColorFactor);
			DisplayTexture(unsigned int, unsigned int, DisplayTexture::ColorFactor);
			~DisplayTexture();

			void setPosition(DisplayTexture::CoordinateX, DisplayTexture::CoordinateY);
			void setSize(float, float, float, float);
			void setFullscreen(bool);

			void display(int, int);

		private:
			void initializeShader();
			void loadGeometry(int, int);

			DisplayTexture::CoordinateX coordinateX;
			DisplayTexture::CoordinateY coordinateY;
			bool fullscreen;
			float userMinX, userMaxX, userMinY, userMaxY;

			//visual
			unsigned int textureID;
			DisplayTexture::ColorFactor colorFactor;

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

			Matrix3<float> mProjection;
			unsigned int displayTextureShader;
			int layer;
			int mProjectionLoc, diffuseTexLoc, colorFactorLoc, layerLoc;
	};

}

#endif

