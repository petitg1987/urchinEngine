#include <GL/gl.h>

#include "SceneDisplayerWidget.h"

namespace urchin
{

	SceneDisplayerWidget::SceneDisplayerWidget(QWidget *parent, const std::string &mapEditorPath) :
			QGLWidget(parent),
			mapEditorPath(mapEditorPath),
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

	MapHandler *SceneDisplayerWidget::newMap(const std::string &mapFilename, const std::string &relativeWorkingDirectory)
	{
		closeMap();

		sceneDisplayer = new SceneDisplayer(this);
		sceneDisplayer->initializeFromNewMap(mapEditorPath, mapFilename, relativeWorkingDirectory);
		sceneDisplayer->resize(this->geometry().width(), this->geometry().height());
		updateSceneDisplayerViewProperties();

		return sceneDisplayer->getMapHandler();
	}

	MapHandler *SceneDisplayerWidget::openMap(const std::string &mapFilename)
	{
		closeMap();

		sceneDisplayer = new SceneDisplayer(this);
		sceneDisplayer->initializeFromExistingMap(mapEditorPath, mapFilename);
		sceneDisplayer->resize(this->geometry().width(), this->geometry().height());
		updateSceneDisplayerViewProperties();

		return sceneDisplayer->getMapHandler();
	}

	void SceneDisplayerWidget::saveMap(const std::string &) const
	{
		//do nothing
	}

	void SceneDisplayerWidget::closeMap()
	{
		delete sceneDisplayer;
		sceneDisplayer=nullptr;
	}

	void SceneDisplayerWidget::setViewProperties(SceneDisplayer::ViewProperties viewProperty, bool value)
	{
		viewProperties[viewProperty] = value;
		updateSceneDisplayerViewProperties();
	}

	void SceneDisplayerWidget::setHighlightSceneObject(const SceneObject *highlightSceneObject)
	{
		if(sceneDisplayer!=nullptr)
		{
			sceneDisplayer->setHighlightSceneObject(highlightSceneObject);
		}
	}

	void SceneDisplayerWidget::setHighlightCompoundShapeComponent(const LocalizedCollisionShape *selectedCompoundShapeComponent)
	{
		if(sceneDisplayer!=nullptr)
		{
			sceneDisplayer->setHighlightCompoundShapeComponent(selectedCompoundShapeComponent);
		}
	}

	void SceneDisplayerWidget::setHighlightSceneLight(const SceneLight *highlightSceneLight)
	{
		if(sceneDisplayer!=nullptr)
		{
			sceneDisplayer->setHighlightSceneLight(highlightSceneLight);
		}
	}

	void SceneDisplayerWidget::setHighlightSceneSound(const SceneSound *highlightSceneSound)
	{
		if(sceneDisplayer!=nullptr)
		{
			sceneDisplayer->setHighlightSceneSound(highlightSceneSound);
		}
	}

	void SceneDisplayerWidget::updateSceneDisplayerViewProperties()
	{
		if(sceneDisplayer!=nullptr)
		{
			for(unsigned int i=0; i<SceneDisplayer::LAST_VIEW_PROPERTIES; ++i)
			{
				sceneDisplayer->setViewProperties(static_cast<SceneDisplayer::ViewProperties>(i), viewProperties[i]);
			}
		}
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
		}else{
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
			sceneDisplayer->getSceneManager()->onKeyDown(KEY_LEFT_ARROW);
		}else if(event->key() == Qt::Key_Right)
		{
			sceneDisplayer->getSceneManager()->onKeyDown(KEY_RIGHT_ARROW);
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
			sceneDisplayer->getSceneManager()->onKeyUp(KEY_LEFT_ARROW);
		}else if(event->key() == Qt::Key_Right)
		{
			sceneDisplayer->getSceneManager()->onKeyUp(KEY_RIGHT_ARROW);
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
			sceneDisplayer->getSceneManager()->onKeyDown(KEY_MOUSE_LEFT);
		}else if (event->button() == Qt::RightButton)
		{
			sceneDisplayer->getSceneManager()->onKeyDown(KEY_MOUSE_RIGHT);
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
			sceneDisplayer->getSceneManager()->onKeyUp(KEY_MOUSE_LEFT);
		}else if (event->button() == Qt::RightButton)
		{
			sceneDisplayer->getSceneManager()->onKeyUp(KEY_MOUSE_RIGHT);
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
