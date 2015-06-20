#include <GL/glew.h>
#include <GL/gl.h>
#include <stdexcept>

#include "SceneManager.h"

namespace urchin
{
	
	SceneManager::SceneManager() :
			bIsInitialized(false),
			sceneWidth(-1),
			sceneHeight(-1),
			previousTime(std::chrono::high_resolution_clock::now()),
			currFrame(0),
			nbrFps(60), //60 fps by default
			indexFps(0),
			refreshRateFps(ConfigService::instance()->getFloatValue("sceneManager.immutable.refreshRateFps"))
	{
		//initialize GL
		initializeGl();

		//3d renderer
		renderer3d = new Renderer3d();

		//GUI manager
		guiManager = new GUIManager();
	}

	SceneManager::~SceneManager()
	{
		//3d renderer
		delete renderer3d;

		//GUI manager
		delete guiManager;
	}

	void SceneManager::initialize()
	{
		if(bIsInitialized)
		{
			throw std::runtime_error("Scene manager is already initialized.");
		}

		//3d renderer
		renderer3d->initialize();

		//GUI manager
		guiManager->initialize();
		guiManager->addObserver(this, GUIManager::ADD_WIDGET);
		guiManager->addObserver(this, GUIManager::REMOVE_WIDGET);

		//fps
		previousFps[0] = nbrFps; previousFps[1] = nbrFps; previousFps[2] = nbrFps;
		previousTime = std::chrono::high_resolution_clock::now();

		bIsInitialized = true;
	}

	bool SceneManager::isInitialized() const
	{
		return bIsInitialized;
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
		if(!bIsInitialized)
		{
			return;
		}

		//scene properties
		sceneWidth = width;
		sceneHeight = height;
		glViewport(0, 0, sceneWidth, sceneHeight);

		//scene 3d
		renderer3d->onResize(width, height);

		//GUI manager
		guiManager->onResize(width, height);
	}

	void SceneManager::notify(Observable *observable, int notificationType)
	{
		if(GUIManager *guiManager = dynamic_cast<GUIManager *>(observable))
		{
			Widget *widget = guiManager->getLastUpdatedWidget();
			switch(notificationType)
			{
				case GUIManager::ADD_WIDGET:
				{
					widget->addObserver(this, Widget::KEYBOARD_LOCKED);
					widget->addObserver(this, Widget::KEYBOARD_UNLOCKED);
					widget->addObserver(this, Widget::ADD_CHILD_WIDGET);
					widget->addObserver(this, Widget::REMOVE_CHILD_WIDGET);
					break;
				}
				case GUIManager::REMOVE_WIDGET:
				{
					widget->removeObserver(this, Widget::KEYBOARD_LOCKED);
					widget->removeObserver(this, Widget::KEYBOARD_UNLOCKED);
					widget->removeObserver(this, Widget::ADD_CHILD_WIDGET);
					widget->removeObserver(this, Widget::REMOVE_CHILD_WIDGET);
					break;
				}
			}
		}else if(Widget *widget = dynamic_cast<Widget *>(observable))
		{
			Widget *childWidget = widget->getLastUpdatedChildWidget();
			switch(notificationType)
			{
				case Widget::ADD_CHILD_WIDGET:
				{
					childWidget->addObserver(this, Widget::KEYBOARD_LOCKED);
					childWidget->addObserver(this, Widget::KEYBOARD_UNLOCKED);
					childWidget->addObserver(this, Widget::ADD_CHILD_WIDGET);
					childWidget->addObserver(this, Widget::REMOVE_CHILD_WIDGET);
					break;
				}
				case Widget::REMOVE_CHILD_WIDGET:
				{
					childWidget->removeObserver(this, Widget::KEYBOARD_LOCKED);
					childWidget->removeObserver(this, Widget::KEYBOARD_UNLOCKED);
					childWidget->removeObserver(this, Widget::ADD_CHILD_WIDGET);
					childWidget->removeObserver(this, Widget::REMOVE_CHILD_WIDGET);
					break;
				}
				case Widget::KEYBOARD_LOCKED:
				{
					renderer3d->onKeyboardLocked(true);
					break;
				}
				case Widget::KEYBOARD_UNLOCKED:
				{
					renderer3d->onKeyboardLocked(false);
					break;
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
		return (nbrFps==0 ? 60 : nbrFps);
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

	Renderer3d *SceneManager::get3dRenderer() const
	{
		return renderer3d;
	}

	GUIManager *SceneManager::getGUIManager() const
	{
		return guiManager;
	}

	TextureManager *SceneManager::getTextureManager() const
	{
		return TextureManager::instance();
	}

	void SceneManager::onKeyDown(unsigned int key)
	{
		renderer3d->onKeyDown(key);
		guiManager->onKeyDown(key);
	}

	void SceneManager::onKeyUp(unsigned int key)
	{
		renderer3d->onKeyUp(key);
		guiManager->onKeyUp(key);
	}

	void SceneManager::onChar(unsigned int character)
	{
		guiManager->onChar(character);
	}

	void SceneManager::onMouseMove(int mouseX, int mouseY)
	{
		renderer3d->onMouseMove(mouseX, mouseY);
		guiManager->onMouseMove(mouseX, mouseY);
	}

	void SceneManager::display()
	{
		if(!bIsInitialized)
		{
			throw std::runtime_error("Scene must be initialized before displayed.");
		}

		computeFps();

		//prepare rendering
		float invFrameRate = getOneOnFps();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		//3d renderer
		renderer3d->display(invFrameRate);

		//GUI manager
		guiManager->display(invFrameRate);
	}

}
