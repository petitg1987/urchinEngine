#ifndef URCHINENGINE_SCENEMANAGER_H
#define URCHINENGINE_SCENEMANAGER_H

#include <chrono>
#include <vector>
#include <array>
#include "UrchinCommon.h"

#include "renderer3d/Renderer3d.h"
#include "GUI/GUIRenderer.h"
#include "texture/TextureManager.h"

#define NUM_RENDERER 2

namespace urchin
{
	class SceneManager
	{
		public:
			SceneManager();
			virtual ~SceneManager();

			//scene properties
			void onResize(unsigned int, unsigned int);
			unsigned int getSceneWidth() const;
			unsigned int getSceneHeight() const;

			//Fps
			float getFps() const;
			unsigned int getFpsForDisplay();
			float getDeltaTime() const;

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
			bool onKeyDown(unsigned int);
			bool onKeyUp(unsigned int);
			bool onChar(unsigned int);
			void onMouseMove(int, int);
			
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
			Renderer *activeRenderers[NUM_RENDERER];
		
			//fps
			std::chrono::high_resolution_clock::time_point previousTime;
			unsigned int indexFps;
			std::array<float, 3> previousFps;
			float fps;
			unsigned int fpsForDisplay;
	};

}

#endif
