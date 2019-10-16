#ifndef URCHINENGINE_SKYBOX_H
#define URCHINENGINE_SKYBOX_H

#include <string>
#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "resources/image/Image.h"
#include "scene/renderer3d/camera/Camera.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{
	
	class Skybox
	{
		public:
			Skybox();
			explicit Skybox(const std::vector<std::string> &);
			~Skybox();

			void onCameraProjectionUpdate(const Camera *);
			void setOffsetY(float);

			void display(const Matrix4<float> &, const Point3<float> &);
		private:
			void initialize();

			Image **texSkybox;
			unsigned int textureID;
			
			float offsetY;
			Matrix4<float> translationMatrix;

			//visual
			std::shared_ptr<QuadDisplayer> quadDisplayer;

			unsigned int skyboxShader;
			int mProjectionLoc, mViewLoc;
	};

}

#endif
