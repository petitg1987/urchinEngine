#include <GL/glew.h>
#include <GL/gl.h>
#include <stdexcept>

#include "SceneManager.h"

#define START_FPS 60
#define NUM_RENDERER 2
#define RENDERER_3D 0
#define GUI_RENDERER 1

namespace urchin
{
	
	SceneManager::SceneManager() :
			sceneWidth(0),
			sceneHeight(0),
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
		for(unsigned int i=0; i<NUM_RENDERER; ++i)
		{
			activeRenderers[i] = nullptr;
		}
	}

	SceneManager::~SceneManager()
	{
		for(unsigned int i=0; i<renderers3d.size(); ++i)
		{
			delete renderers3d[i];
		}

		for(unsigned int i=0; i<guiRenderers.size(); ++i)
		{
			delete guiRenderers[i];
		}
	}

	void SceneManager::initializeGl()
	{
		//initialization Glew
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			throw std::runtime_error((char *)glewGetErrorString(err));
		}

		//initialization OpenGl
		glEnable(GL_DEPTH_TEST);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		//check OpenGL context version
		int majorVersionContext = 0, minorVersionContext = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &majorVersionContext);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersionContext);

		if((majorVersionContext*100 + minorVersionContext*10) < 330)
		{
			std::ostringstream ossMajorVersionContext;
			ossMajorVersionContext << majorVersionContext;

			std::ostringstream ossMinorVersionContext;
			ossMinorVersionContext << minorVersionContext;

			throw std::runtime_error("OpenGL version require: 3.3, current version: " + ossMajorVersionContext.str() + "." + ossMinorVersionContext.str() + ".");
		}
	}

	void SceneManager::onResize(unsigned int width, unsigned int height)
	{
		//scene properties
		sceneWidth = width;
		sceneHeight = height;
		glViewport(0, 0, sceneWidth, sceneHeight);

		//renderer
		for(unsigned int i=0; i<NUM_RENDERER; ++i)
		{
			if(activeRenderers[i]!=nullptr)
			{
				activeRenderers[i]->onResize(sceneWidth, sceneHeight);
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
		Renderer3d *renderer3d = new Renderer3d();
		renderers3d.push_back(renderer3d);

		if(enable)
		{
			enableRenderer3d(renderer3d);
		}
		return renderer3d;
	}

	void SceneManager::enableRenderer3d(Renderer3d *renderer3d)
	{
		activeRenderers[RENDERER_3D] = renderer3d;
		if(renderer3d!=nullptr)
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
		return static_cast<Renderer3d *>(activeRenderers[RENDERER_3D]);
	}

	GUIRenderer *SceneManager::newGUIRenderer(bool enable)
	{
		GUIRenderer *guiRenderer = new GUIRenderer();
		guiRenderers.push_back(guiRenderer);

		if(enable)
		{
			enableGUIRenderer(guiRenderer);
		}
		return guiRenderer;
	}

	void SceneManager::enableGUIRenderer(GUIRenderer *guiRenderer)
	{
		activeRenderers[GUI_RENDERER] = guiRenderer;
		if(guiRenderer!=nullptr)
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
		return static_cast<GUIRenderer *>(activeRenderers[GUI_RENDERER]);
	}

	TextureManager *SceneManager::getTextureManager() const
	{
		return TextureManager::instance();
	}

	bool SceneManager::onKeyDown(unsigned int key)
	{
		for(int i=NUM_RENDERER-1; i>=0; --i)
		{
			if(activeRenderers[i]!=nullptr && !activeRenderers[i]->onKeyDown(key))
			{
				return false;
			}
		}
		return false;
	}

	bool SceneManager::onKeyUp(unsigned int key)
	{
		for(int i=NUM_RENDERER-1; i>=0; --i)
		{
			if(activeRenderers[i]!=nullptr && !activeRenderers[i]->onKeyUp(key))
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
			if(activeRenderers[i]!=nullptr && !activeRenderers[i]->onChar(character))
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
			if(activeRenderers[i]!=nullptr && !activeRenderers[i]->onMouseMove(mouseX, mouseY))
			{
				break;
			}
		}
	}

	void SceneManager::display()
	{
		//FPS
		computeFps();
		float invFrameRate = getOneOnFps();

		//renderer
		for(unsigned int i=0; i<NUM_RENDERER; ++i)
		{
			if(activeRenderers[i]!=nullptr)
			{
				activeRenderers[i]->display(invFrameRate);
			}
		}

		#ifdef _DEBUG
			GLenum err = GL_NO_ERROR;
			while((err = glGetError()) != GL_NO_ERROR)
			{
				std::cout<<"OpenGL error: "<<err<<std::endl;
			}
		#endif
	}

}
