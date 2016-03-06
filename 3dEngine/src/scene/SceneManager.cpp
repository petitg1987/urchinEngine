#include <GL/glew.h>
#include <GL/gl.h>
#include <stdexcept>

#include "SceneManager.h"

#define START_FPS 60

namespace urchin
{
	
	SceneManager::SceneManager() :
			sceneWidth(-1),
			sceneHeight(-1),
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
	}

	SceneManager::~SceneManager()
	{
		for(std::map<std::string, Renderer3d *>::iterator it = renderers3d.begin(); it!=renderers3d.end(); ++it)
		{
			delete it->second;
		}

		for(std::map<std::string, GUIRenderer *>::iterator it = guiRenderers.begin(); it!=guiRenderers.end(); ++it)
		{
			delete it->second;
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

	void SceneManager::onResize(int width, int height)
	{
		//scene properties
		sceneWidth = width;
		sceneHeight = height;
		glViewport(0, 0, sceneWidth, sceneHeight);

		//renderer
		if(activeRenderer3d)
		{
			activeRenderer3d->onResize(width, height);
		}
		if(activeGUIRenderer)
		{
			activeGUIRenderer->onResize(width, height);
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

	Renderer3d *SceneManager::newRenderer3d(const std::string &name, bool activeIt)
	{
		std::map<std::string, Renderer3d *>::const_iterator it = renderers3d.find(name);
		if(it!=renderers3d.end())
		{
			throw std::invalid_argument("Renderer 3d with following name already exist: " + name);
		}

		Renderer3d *renderer3d = new Renderer3d();
		renderers3d[name] = renderer3d;

		if(activeIt)
		{
			enableRenderer3d(name);
		}

		return renderer3d;
	}

	Renderer3d *SceneManager::enableRenderer3d(const std::string &name)
	{
		std::map<std::string, Renderer3d *>::const_iterator it = renderers3d.find(name);
		if(it==renderers3d.end())
		{
			throw std::invalid_argument("Impossible to find a renderer 3d named " + name);
		}

		activeRenderer3d = it->second;
		return activeRenderer3d;
	}

	void SceneManager::disableActiveRenderer3d()
	{
		activeRenderer3d = nullptr;
	}

	void SceneManager::removeRenderer3d(const std::string &name)
	{
		std::map<std::string, Renderer3d *>::const_iterator it = renderers3d.find(name);
		if(it!=renderers3d.end())
		{
			if(it->second==activeRenderer3d)
			{
				disableActiveRenderer3d();
			}
			delete it->second;
		}
	}

	Renderer3d *SceneManager::getActiveRenderer3d() const
	{
		return activeRenderer3d;
	}

	GUIRenderer *SceneManager::newGUIRenderer(const std::string &name, bool activeIt)
	{
		std::map<std::string, GUIRenderer *>::const_iterator it = guiRenderers.find(name);
		if(it!=guiRenderers.end())
		{
			throw std::invalid_argument("GUI renderer with following name already exist: " + name);
		}

		GUIRenderer *guiRenderer = new GUIRenderer();
		guiRenderers[name] = guiRenderer;

		if(activeIt)
		{
			enableGUIRenderer(name);
		}

		return guiRenderer;
	}

	GUIRenderer *SceneManager::enableGUIRenderer(const std::string &name = "")
	{
		std::map<std::string, GUIRenderer *>::const_iterator it = guiRenderers.find(name);
		if(it==guiRenderers.end())
		{
			throw std::invalid_argument("Impossible to find a GUI renderer named " + name);
		}

		activeGUIRenderer = it->second;
		return activeGUIRenderer;
	}

	void SceneManager::disableActiveGUIRenderer()
	{
		activeGUIRenderer = nullptr;
	}

	void SceneManager::removeGUIRenderer3d(const std::string &name)
	{
		std::map<std::string, GUIRenderer *>::const_iterator it = guiRenderers.find(name);
		if(it!=guiRenderers.end())
		{
			if(it->second==activeGUIRenderer)
			{
				disableActiveGUIRenderer();
			}
			delete it->second;
		}
	}

	GUIRenderer *SceneManager::getActiveGUIRenderer() const
	{
		return activeGUIRenderer;
	}

	TextureManager *SceneManager::getTextureManager() const
	{
		return TextureManager::instance();
	}

	void SceneManager::onKeyDown(unsigned int key)
	{
		bool propagateEvent = true;
		if(activeGUIRenderer)
		{
			propagateEvent = activeGUIRenderer->onKeyDown(key);
		}

		if(activeRenderer3d && propagateEvent)
		{
			activeRenderer3d->onKeyDown(key);
		}
	}

	void SceneManager::onKeyUp(unsigned int key)
	{
		bool propagateEvent = true;
		if(activeGUIRenderer)
		{
			propagateEvent = activeGUIRenderer->onKeyUp(key);
		}

		if(activeRenderer3d && propagateEvent)
		{
			activeRenderer3d->onKeyUp(key);
		}
	}

	void SceneManager::onChar(unsigned int character)
	{
		if(activeGUIRenderer)
		{
			activeGUIRenderer->onChar(character);
		}
	}

	void SceneManager::onMouseMove(int mouseX, int mouseY)
	{
		bool propagateEvent = true;
		if(activeGUIRenderer)
		{
			propagateEvent = activeGUIRenderer->onMouseMove(mouseX, mouseY);
		}

		if(activeRenderer3d && propagateEvent)
		{
			activeRenderer3d->onMouseMove(mouseX, mouseY);
		}
	}

	void SceneManager::display()
	{
		computeFps();
		float invFrameRate = getOneOnFps();

		//renderer
		if(activeRenderer3d)
		{
			activeRenderer3d->display(invFrameRate);
		}
		if(activeGUIRenderer)
		{
			activeGUIRenderer->display(invFrameRate);
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
