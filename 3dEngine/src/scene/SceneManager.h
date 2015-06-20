#ifndef ENGINE_SCENEMANAGER_H
#define ENGINE_SCENEMANAGER_H

#include <chrono>
#include "UrchinCommon.h"

#include "renderer3d/Renderer3d.h"
#include "renderer3d/camera/Camera.h"
#include "GUI/GUIManager.h"
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

	class SceneManager : public Observer
	{
		public:
			SceneManager();
			virtual ~SceneManager();

			//scene properties
			void initialize();
			bool isInitialized() const;
			void onResize(int, int);
			void notify(Observable *, int);
			int getSceneWidth() const;
			int getSceneHeight() const;

			//fps
			int getFps() const;
			float getOneOnFps() const;

			//managers
			Renderer3d *get3dRenderer() const;
			GUIManager *getGUIManager() const;
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
			bool bIsInitialized;
			int sceneWidth, sceneHeight;

			//3d renderer
			Renderer3d *renderer3d;

			//GUI manager
			GUIManager *guiManager;
		
			//fps
			std::chrono::high_resolution_clock::time_point previousTime;
			int currFrame, nbrFps, previousFps[3], indexFps;
			float refreshRateFps;
	};

}

#endif
