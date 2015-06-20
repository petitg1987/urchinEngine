#ifndef ENGINE_SKYBOX_H
#define ENGINE_SKYBOX_H

#include <string>
#include <vector>
#include "UrchinCommon.h"

#include "resources/image/Image.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin
{
	
	class Skybox
	{
		public:
			Skybox();
			Skybox(const std::vector<std::string> &);
			~Skybox();

			void onCameraProjectionUpdate(const Camera *const);
			void setOffsetY(float);

			void display(const Matrix4<float> &, const Point3<float> &);
		private:
			void initialize();

			Image **texSkybox;
			unsigned int textureID;
			
			float offsetY;
			Matrix4<float> translationMatrix;

			//visual
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

			unsigned int skyboxShader;
			int mProjectionLoc, mViewLoc, diffuseTexSamplerLoc;
	};

}

#endif
