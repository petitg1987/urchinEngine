#ifndef URCHINENGINE_TEXTUREDISPLAYER_H
#define URCHINENGINE_TEXTUREDISPLAYER_H

#include <memory>
#include "UrchinCommon.h"

#include "utils/display/quad/QuadDisplayer.h"

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
	class TextureDisplayer
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

			enum ColorType
			{
				DEFAULT_VALUE,
				DEPTH_VALUE,
				GRAYSCALE_VALUE,
				INVERSE_GRAYSCALE_VALUE
			};

			TextureDisplayer(unsigned int, TextureDisplayer::ColorType, float colorIntensity = 1.0);
			TextureDisplayer(unsigned int, unsigned int, TextureDisplayer::ColorType, float colorIntensity = 1.0);
			~TextureDisplayer();

			void setPosition(TextureDisplayer::CoordinateX, TextureDisplayer::CoordinateY);
			void setSize(float, float, float, float);
			void setFullScreen(bool);

			void initialize(unsigned int, unsigned int, float, float);

			void display();

		private:
			void initializeShader(float, float);

			bool isInitialized;

			TextureDisplayer::CoordinateX coordinateX;
			TextureDisplayer::CoordinateY coordinateY;
			bool fullScreen;
			float userMinX, userMaxX, userMinY, userMaxY;

			//visual
			unsigned int textureID;
			TextureDisplayer::ColorType colorType;
			float colorIntensity;
			std::shared_ptr<QuadDisplayer> quadDisplayer;

			Matrix3<float> mProjection;
			unsigned int displayTextureShader;
			int layer;
			int mProjectionLoc, diffuseTexLoc;
	};

}

#endif

