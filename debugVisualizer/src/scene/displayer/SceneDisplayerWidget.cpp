#include <GL/gl.h>

#include "SceneDisplayerWidget.h"

namespace urchin
{

	SceneDisplayerWidget::SceneDisplayerWidget(QWidget *parent, const std::string &debugVisualizerPath) :
			QGLWidget(parent),
			debugVisualizerPath(debugVisualizerPath),
			sceneDisplayer(nullptr)
	{
		QGLFormat glFormat;
		glFormat.setVersion(3,3);
		glFormat.setProfile(QGLFormat::CompatibilityProfile);
		glFormat.setSampleBuffers(true);
		glFormat.setDoubleBuffer(true);
		setFormat(glFormat);
	}

	SceneDisplayerWidget::~SceneDisplayerWidget()
	{
		delete sceneDisplayer;
	}

	void SceneDisplayerWidget::openDebugFile(const std::string &debugFilename)
	{
		sceneDisplayer = new SceneDisplayer(this);
		sceneDisplayer->initializeFromDebugFile(debugVisualizerPath, debugFilename);
		sceneDisplayer->resize(this->geometry().width(), this->geometry().height());
	}

	void SceneDisplayerWidget::initializeGL()
	{
		//do nothing
	}

	void SceneDisplayerWidget::paintGL()
	{
		if(sceneDisplayer!=nullptr)
		{
			sceneDisplayer->paint();
		}else
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0.1f, 0.05f, 0.1f, 1.0f);
		}

		swapBuffers();
		update();
	}

	void SceneDisplayerWidget::resizeGL(int widget, int height)
	{
		if(sceneDisplayer!=nullptr)
		{
			sceneDisplayer->resize(widget, height);
		}
	}

	void SceneDisplayerWidget::keyPressEvent(QKeyEvent *event)
	{
		if(sceneDisplayer==nullptr)
		{
			return;
		}

		if(event->key() < 256)
		{
			sceneDisplayer->getSceneManager()->onKeyDown(event->key());
			sceneDisplayer->getSceneManager()->onChar(event->text().toLatin1()[0]);
		}else if(event->key() == Qt::Key_Left)
		{
			sceneDisplayer->getSceneManager()->onKeyDown(InputDevice::Key::LEFT_ARROW);
		}else if(event->key() == Qt::Key_Right)
		{
			sceneDisplayer->getSceneManager()->onKeyDown(InputDevice::Key::RIGHT_ARROW);
		}
		else if(event->key() == Qt:: Key_Backspace)
		{
			sceneDisplayer->getSceneManager()->onChar(8);
		}else if(event->key() == Qt:: Key_Delete)
		{
			sceneDisplayer->getSceneManager()->onChar(127);
		}
	}

	void SceneDisplayerWidget::keyReleaseEvent(QKeyEvent *event)
	{
		if(sceneDisplayer==nullptr)
		{
			return;
		}

		if(event->key() < 256)
		{
			sceneDisplayer->getSceneManager()->onKeyUp(event->key());
		}else if(event->key() == Qt::Key_Left)
		{
			sceneDisplayer->getSceneManager()->onKeyUp(InputDevice::Key::LEFT_ARROW);
		}else if(event->key() == Qt::Key_Right)
		{
			sceneDisplayer->getSceneManager()->onKeyUp(InputDevice::Key::RIGHT_ARROW);
		}
	}

	void SceneDisplayerWidget::mousePressEvent(QMouseEvent *event)
	{
		if(sceneDisplayer==nullptr)
		{
			return;
		}

		if(event->buttons() == Qt::LeftButton)
		{
			sceneDisplayer->getSceneManager()->onKeyDown(InputDevice::Key::MOUSE_LEFT);
		}else if (event->button() == Qt::RightButton)
		{
			sceneDisplayer->getSceneManager()->onKeyDown(InputDevice::Key::MOUSE_RIGHT);
		}
	}

	void SceneDisplayerWidget::mouseReleaseEvent(QMouseEvent *event)
	{
		if(sceneDisplayer==nullptr)
		{
			return;
		}

		if(event->button() == Qt::LeftButton)
		{
			sceneDisplayer->getSceneManager()->onKeyUp(InputDevice::Key::MOUSE_LEFT);
		}else if (event->button() == Qt::RightButton)
		{
			sceneDisplayer->getSceneManager()->onKeyUp(InputDevice::Key::MOUSE_RIGHT);
		}
	}

	void SceneDisplayerWidget::mouseMoveEvent(QMouseEvent *event)
	{
		if(sceneDisplayer!=nullptr)
		{
			sceneDisplayer->getSceneManager()->onMouseMove(event->x(), event->y());
		}
	}

}
