#include <GL/glew.h>
#include <stdexcept>

#include "SceneManager.h"

#define START_FPS 60
#define RENDERER_3D 0
#define GUI_RENDERER 1

namespace urchin
{
	
	SceneManager::SceneManager() :
			sceneWidth(500),
			sceneHeight(500),
			previousTime(std::chrono::high_resolution_clock::now()),
			currFrame(0),
			nbrFps(START_FPS),
			indexFps(0),
			refreshRateFps(ConfigService::instance()->getFloatValue("sceneManager.refreshRateFps"))
	{
		//initialize GL
		initializeGl();

		//initialize FPS
		previousFps[0] = nbrFps; previousFps[1] = nbrFps; previousFps[2] = nbrFps;
		previousTime = std::chrono::high_resolution_clock::now();

		//renderer
		for (auto &activeRenderer : activeRenderers)
		{
			activeRenderer = nullptr;
		}
	}

	SceneManager::~SceneManager()
	{
		for (auto &renderer3d : renderers3d)
		{
			delete renderer3d;
		}

		for (auto &guiRenderer : guiRenderers)
		{
			delete guiRenderer;
		}

		Profiler::getInstance("3d")->log();
	}

	void SceneManager::initializeGl()
	{
		//initialization Glew
		GLenum err = glewInit();
		if(err != GLEW_OK)
		{
			throw std::runtime_error((char *)glewGetErrorString(err));
		}

		//check OpenGL version supported
		if(!glewIsSupported("GL_VERSION_4_4"))
		{
			throw std::runtime_error("OpenGL version 4.4 is required but it's not supported on this environment.");
		}

		//check OpenGL context version
		int majorVersionContext = 0, minorVersionContext = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &majorVersionContext);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersionContext);

		if((majorVersionContext*100 + minorVersionContext*10) < 440)
		{
			std::ostringstream ossMajorVersionContext;
			ossMajorVersionContext << majorVersionContext;

			std::ostringstream ossMinorVersionContext;
			ossMinorVersionContext << minorVersionContext;

			throw std::runtime_error("OpenGL context version required: 4.4, current version: " + ossMajorVersionContext.str() + "." + ossMinorVersionContext.str() + ".");
		}

		//initialization OpenGl
		glEnable(GL_DEPTH_TEST);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}

	void SceneManager::onResize(unsigned int width, unsigned int height)
	{
		if(width!=0 && height!=0)
		{
			//scene properties
			sceneWidth = width;
			sceneHeight = height;
			glViewport(0, 0, sceneWidth, sceneHeight);

			//renderer
			for (auto &activeRenderer : activeRenderers)
			{
				if(activeRenderer)
				{
					activeRenderer->onResize(sceneWidth, sceneHeight);
				}
			}
		}
	}

	int SceneManager::getSceneWidth() const
	{
		return sceneWidth;
	}

	int SceneManager::getSceneHeight() const
	{
		return sceneHeight;
	}

	int SceneManager::getFps() const
	{
		return (nbrFps==0 ? START_FPS : nbrFps);
	}

	float SceneManager::getOneOnFps() const
	{
		return (1.0/(float)getFps());
	}

	void SceneManager::computeFps()
	{
		currFrame++;
		std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		int timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime).count();

		if(timeInterval > refreshRateFps)
		{
			previousFps[indexFps%3] = (int)(currFrame / (timeInterval/1000.0f));
			indexFps++;

			nbrFps = (int)((previousFps[(indexFps-0)%3]*2 + previousFps[(indexFps-1)%3]*2 + previousFps[(indexFps-2)%3]*1) / 5.0);

			previousTime = currentTime;
			currFrame = 0;
		}
	}

	Renderer3d *SceneManager::newRenderer3d(bool enable)
	{
		auto *renderer3d = new Renderer3d();
		renderers3d.push_back(renderer3d);

		if(enable)
		{
			enableRenderer3d(renderer3d);
		}
		return renderer3d;
	}

	void SceneManager::enableRenderer3d(Renderer3d *renderer3d)
	{
		if(activeRenderers[RENDERER_3D])
		{
			activeRenderers[RENDERER_3D]->onDisable();
		}

		activeRenderers[RENDERER_3D] = renderer3d;
		if(renderer3d)
		{
			renderer3d->onResize(sceneWidth, sceneHeight);
		}
	}

	void SceneManager::removeRenderer3d(Renderer3d *renderer3d)
	{
		if(activeRenderers[RENDERER_3D] == renderer3d)
		{
			activeRenderers[RENDERER_3D] = nullptr;
		}

		renderers3d.erase(std::remove(renderers3d.begin(), renderers3d.end(), renderer3d), renderers3d.end());
		delete renderer3d;
	}

	Renderer3d *SceneManager::getActiveRenderer3d() const
	{
		return dynamic_cast<Renderer3d *>(activeRenderers[RENDERER_3D]);
	}

	GUIRenderer *SceneManager::newGUIRenderer(bool enable)
	{
		auto *guiRenderer = new GUIRenderer();
		guiRenderers.push_back(guiRenderer);

		if(enable)
		{
			enableGUIRenderer(guiRenderer);
		}
		return guiRenderer;
	}

	void SceneManager::enableGUIRenderer(GUIRenderer *guiRenderer)
	{
		if(activeRenderers[GUI_RENDERER])
		{
			activeRenderers[GUI_RENDERER]->onDisable();
		}

		activeRenderers[GUI_RENDERER] = guiRenderer;
		if(guiRenderer)
		{
			guiRenderer->onResize(sceneWidth, sceneHeight);
		}
	}

	void SceneManager::removeGUIRenderer(GUIRenderer *guiRenderer)
	{
		if(activeRenderers[GUI_RENDERER] == guiRenderer)
		{
			activeRenderers[GUI_RENDERER] = nullptr;
		}

		guiRenderers.erase(std::remove(guiRenderers.begin(), guiRenderers.end(), guiRenderer), guiRenderers.end());
		delete guiRenderer;
	}

	GUIRenderer *SceneManager::getActiveGUIRenderer() const
	{
		return dynamic_cast<GUIRenderer *>(activeRenderers[GUI_RENDERER]);
	}

	TextureManager *SceneManager::getTextureManager() const
	{
		return TextureManager::instance();
	}

	bool SceneManager::onKeyDown(unsigned int key)
	{
		for(int i=NUM_RENDERER-1; i>=0; --i)
		{
			if(activeRenderers[i] && !activeRenderers[i]->onKeyDown(key))
			{
				return false;
			}
		}
		return true;
	}

	bool SceneManager::onKeyUp(unsigned int key)
	{
		for(int i=NUM_RENDERER-1; i>=0; --i)
		{
			if(activeRenderers[i] && !activeRenderers[i]->onKeyUp(key))
			{
				return false;
			}
		}
		return true;
	}

	bool SceneManager::onChar(unsigned int character)
	{
		for(int i=NUM_RENDERER-1; i>=0; --i)
		{
			if(activeRenderers[i] && !activeRenderers[i]->onChar(character))
			{
				return false;
			}
		}
		return true;
	}

	void SceneManager::onMouseMove(int mouseX, int mouseY)
	{
		for(int i=NUM_RENDERER-1; i>=0; --i)
		{
			if(activeRenderers[i] && !activeRenderers[i]->onMouseMove(mouseX, mouseY))
			{
				break;
			}
		}
	}

	void SceneManager::display()
	{
		ScopeProfiler profiler("3d", "sceneMgrDisplay");

		//FPS
		computeFps();
		float invFrameRate = getOneOnFps();

		//renderer
		for (auto &activeRenderer : activeRenderers)
		{
			if(activeRenderer)
			{
				activeRenderer->display(invFrameRate);
			}
		}

		GLenum err;
		while((err = glGetError()) != GL_NO_ERROR)
		{
			throw std::runtime_error("OpenGL error: " + std::to_string(err));
		}
	}

}
