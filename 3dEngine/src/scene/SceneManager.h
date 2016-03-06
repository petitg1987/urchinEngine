#ifndef ENGINE_SCENEMANAGER_H
#define ENGINE_SCENEMANAGER_H

#include <chrono>
#include <vector>
#include <string>
#include "UrchinCommon.h"

#include "renderer3d/Renderer3d.h"
#include "renderer3d/camera/Camera.h"
#include "GUI/GUIRenderer.h"
#include "texture/TextureManager.h"

namespace urchin
{

	enum
	{ //enumeration for keys
		KEY_MOUSE_LEFT = 256,
		KEY_MOUSE_RIGHT,
		KEY_LEFT_ARROW,
		KEY_RIGHT_ARROW,
	};

	class SceneManager
	{
		public:
			SceneManager();
			virtual ~SceneManager();

			//scene properties
			void onResize(unsigned int, unsigned int);
			int getSceneWidth() const;
			int getSceneHeight() const;

			//FPS
			int getFps() const;
			float getOneOnFps() const;

			//renderer
			Renderer3d *newRenderer3d(bool);
			void enableRenderer3d(Renderer3d *);
			void removeRenderer3d(Renderer3d *);
			Renderer3d *getActiveRenderer3d() const;

			GUIRenderer *newGUIRenderer(bool);
			void enableGUIRenderer(GUIRenderer *);
			void removeGUIRenderer(GUIRenderer *);
			GUIRenderer *getActiveGUIRenderer() const;

			//texture manager
			TextureManager *getTextureManager() const;

			//events
			void onKeyDown(unsigned int);
			void onKeyUp(unsigned int);
			void onChar(unsigned int);
			void onMouseMove(int, int);
			int getMousePositionX();
			int getMousePositionY();
			
			//scene
			void display();

		private:
			void initializeGl();
			void computeFps();
		
			//scene properties
			unsigned int sceneWidth, sceneHeight;

			//renderer
			std::vector<Renderer3d *> renderers3d;
			std::vector<GUIRenderer *> guiRenderers;
			Renderer *activeRenderers[2];
		
			//FPS
			std::chrono::high_resolution_clock::time_point previousTime;
			int currFrame, nbrFps, previousFps[3], indexFps;
			float refreshRateFps;
	};

}

#endif
