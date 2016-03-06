#ifndef ENGINE_SCENEMANAGER_H
#define ENGINE_SCENEMANAGER_H

#include <chrono>
#include <map>
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
			void onResize(int, int);
			int getSceneWidth() const;
			int getSceneHeight() const;

			//fps
			int getFps() const;
			float getOneOnFps() const;

			//renderer
			Renderer3d *newRenderer3d(const std::string &, bool);
			Renderer3d *enableRenderer3d(const std::string &name);
			void disableActiveRenderer3d();
			void removeRenderer3d(const std::string &);
			Renderer3d *getActiveRenderer3d() const;

			GUIRenderer *newGUIRenderer(const std::string &, bool);
			GUIRenderer *enableGUIRenderer(const std::string &name);
			void disableActiveGUIRenderer();
			void removeGUIRenderer3d(const std::string &);
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
			int sceneWidth, sceneHeight;

			//renderer
			std::map<std::string, Renderer3d *> renderers3d;
			Renderer3d *activeRenderer3d;

			std::map<std::string, GUIRenderer *> guiRenderers;
			GUIRenderer *activeGUIRenderer;
		
			//fps
			std::chrono::high_resolution_clock::time_point previousTime;
			int currFrame, nbrFps, previousFps[3], indexFps;
			float refreshRateFps;
	};

}

#endif
