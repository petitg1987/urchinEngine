#include <GL/glew.h>
#include <utility>
#include <QtWidgets/QShortcut>

#include "SceneDisplayerWidget.h"
#include "widget/controller/mouse/MouseController.h"

#define PICKING_RAY_LENGTH 100.0f

namespace urchin
{

	SceneDisplayerWidget::SceneDisplayerWidget(QWidget *parent, const StatusBarController &statusBarController, std::string mapEditorPath) :
			QGLWidget(parent),
            statusBarController(statusBarController),
			mapEditorPath(std::move(mapEditorPath)),
			sceneDisplayer(nullptr),
            viewProperties(),
            mouseX(0),
            mouseY(0)
	{
		QGLFormat glFormat;
		glFormat.setVersion(4, 4);
		glFormat.setProfile(QGLFormat::CompatibilityProfile);
		glFormat.setSampleBuffers(true);
		glFormat.setDoubleBuffer(true);
		glFormat.setSwapInterval(1); //vertical sync
		setFormat(glFormat);

		context()->makeCurrent();

        QObject::connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_X), parent), SIGNAL(activated()), this, SLOT(onCtrlXPressed()));
        QObject::connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y), parent), SIGNAL(activated()), this, SLOT(onCtrlYPressed()));
        QObject::connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z), parent), SIGNAL(activated()), this, SLOT(onCtrlZPressed()));
	}

	SceneDisplayerWidget::~SceneDisplayerWidget()
	{
	    if(context()->isValid())
        {
            context()->doneCurrent();
        }

		delete sceneDisplayer;
	}

    void SceneDisplayerWidget::loadMap(SceneController *sceneController, const std::string &mapFilename, const std::string &relativeWorkingDirectory)
    {
        closeMap();
        statusBarController.applyState(StatusBarState::MAP_LOADED);

        sceneDisplayer = new SceneDisplayer(sceneController, MouseController(this), statusBarController);
        sceneDisplayer->loadMap(mapEditorPath, mapFilename, relativeWorkingDirectory);
        sceneDisplayer->resize(static_cast<unsigned int>(geometry().width()), static_cast<unsigned int>(geometry().height()));
        sceneController->setup(sceneDisplayer->getMapHandler());
        updateSceneDisplayerViewProperties();
    }

	void SceneDisplayerWidget::saveState(const std::string &mapFilename) const
	{
		if(sceneDisplayer)
        {
		    sceneDisplayer->getCamera()->saveCameraState(mapFilename);
        }
	}

	void SceneDisplayerWidget::closeMap()
	{
	    statusBarController.clearState();

		delete sceneDisplayer;
		sceneDisplayer = nullptr;
	}

	void SceneDisplayerWidget::setViewProperties(SceneDisplayer::ViewProperties viewProperty, bool value)
	{
		viewProperties[viewProperty] = value;
		updateSceneDisplayerViewProperties();
	}

	void SceneDisplayerWidget::setHighlightSceneObject(const SceneObject *highlightSceneObject)
	{
		if(sceneDisplayer)
		{
			sceneDisplayer->setHighlightSceneObject(highlightSceneObject);
		}
	}

	void SceneDisplayerWidget::setHighlightCompoundShapeComponent(std::shared_ptr<const LocalizedCollisionShape> selectedCompoundShapeComponent)
	{
		if(sceneDisplayer)
		{
			sceneDisplayer->getBodyShapeDisplayer()->setSelectedCompoundShapeComponent(std::move(selectedCompoundShapeComponent));
		}
	}

	void SceneDisplayerWidget::setHighlightSceneLight(const SceneLight *highlightSceneLight)
	{
		if(sceneDisplayer)
		{
			sceneDisplayer->setHighlightSceneLight(highlightSceneLight);
		}
	}

	void SceneDisplayerWidget::setHighlightSceneSound(const SceneSound *highlightSceneSound)
	{
		if(sceneDisplayer)
		{
			sceneDisplayer->setHighlightSceneSound(highlightSceneSound);
		}
	}

	void SceneDisplayerWidget::updateSceneDisplayerViewProperties()
	{
		if(sceneDisplayer)
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
		if(sceneDisplayer)
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
		if(sceneDisplayer)
		{
			sceneDisplayer->resize(static_cast<unsigned int>(widget), static_cast<unsigned int>(height));
		}
	}

	void SceneDisplayerWidget::keyPressEvent(QKeyEvent *event)
	{
		if(sceneDisplayer)
        {
            if (event->key() < 256)
            {
                sceneDisplayer->getSceneManager()->onKeyDown(static_cast<unsigned int>(event->key()));
                sceneDisplayer->getSceneManager()->onChar(static_cast<unsigned int>(event->text().toLatin1()[0]));
            } else if (event->key() == Qt::Key_Left)
            {
                sceneDisplayer->getSceneManager()->onKeyDown(InputDevice::Key::LEFT_ARROW);
            } else if (event->key() == Qt::Key_Right)
            {
                sceneDisplayer->getSceneManager()->onKeyDown(InputDevice::Key::RIGHT_ARROW);
            } else if (event->key() == Qt::Key_Backspace)
            {
                sceneDisplayer->getSceneManager()->onChar(8);
            } else if (event->key() == Qt::Key_Delete)
            {
                sceneDisplayer->getSceneManager()->onChar(127);
            }
        }
	}

	void SceneDisplayerWidget::keyReleaseEvent(QKeyEvent *event)
	{
		if(sceneDisplayer)
		{
            if(event->key() < 256)
            {
                sceneDisplayer->getSceneManager()->onKeyUp(static_cast<unsigned int>(event->key()));
            }else if(event->key() == Qt::Key_Left)
            {
                sceneDisplayer->getSceneManager()->onKeyUp(InputDevice::Key::LEFT_ARROW);
            }else if(event->key() == Qt::Key_Right)
            {
                sceneDisplayer->getSceneManager()->onKeyUp(InputDevice::Key::RIGHT_ARROW);
            }else if(event->key() == Qt::Key_Escape)
            {
                sceneDisplayer->getObjectMoveController()->onEscapeKey();
            }
		}
	}

	void SceneDisplayerWidget::mousePressEvent(QMouseEvent *event)
	{
		if(sceneDisplayer)
		{
            if(event->buttons() == Qt::LeftButton)
            {
                sceneDisplayer->getSceneManager()->onKeyDown(InputDevice::Key::MOUSE_LEFT);
            }else if (event->button() == Qt::RightButton)
            {
                sceneDisplayer->getSceneManager()->onKeyDown(InputDevice::Key::MOUSE_RIGHT);
            }
		}
	}

	void SceneDisplayerWidget::mouseReleaseEvent(QMouseEvent *event)
	{
		if(sceneDisplayer)
		{
            if(event->button() == Qt::LeftButton)
            {
                bool propagateEvent = sceneDisplayer->getObjectMoveController()->onMouseLeftButton();
                if(propagateEvent)
                {
                    propagateEvent = onMouseClickBodyPickup();
                    if(propagateEvent)
                    {
                        sceneDisplayer->getSceneManager()->onKeyUp(InputDevice::Key::MOUSE_LEFT);
                    }
                }
            }else if (event->button() == Qt::RightButton)
            {
                sceneDisplayer->getSceneManager()->onKeyUp(InputDevice::Key::MOUSE_RIGHT);
            }
		}
	}

	void SceneDisplayerWidget::mouseMoveEvent(QMouseEvent *event)
	{
        this->mouseX = event->x();
        this->mouseY = event->y();

		if(sceneDisplayer)
		{
            bool propagateEvent = sceneDisplayer->getSceneManager()->onMouseMove(mouseX, mouseY);
		    if(propagateEvent)
            {
                sceneDisplayer->getObjectMoveController()->onMouseMove(mouseX, mouseY);
            }
		}
	}

    bool SceneDisplayerWidget::onMouseClickBodyPickup()
    {
        bool propagateEvent = true;

        Camera *camera = sceneDisplayer->getSceneManager()->getActiveRenderer3d()->getCamera();
        Ray<float> pickingRay = CameraSpaceService(camera).screenPointToRay(Point2<float>(mouseX, mouseY), PICKING_RAY_LENGTH);
        std::shared_ptr<const RayTestResult> rayTestResult = sceneDisplayer->getPhysicsWorld()->rayTest(pickingRay);

        while(!rayTestResult->isResultReady())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        const ccd_set &pickedObjects = rayTestResult->getResults();
        if(!pickedObjects.empty())
        {
            lastPickedBodyId = (*rayTestResult->getResults().begin())->getBody2()->getId();
            notifyObservers(this, BODY_PICKED);
            propagateEvent = false;
        } else
        {
            lastPickedBodyId = "";
            notifyObservers(this, BODY_PICKED);
        }

        return propagateEvent;
    }

	const std::string &SceneDisplayerWidget::getLastPickedBodyId() const
	{
		return lastPickedBodyId;
	}

    void SceneDisplayerWidget::addObserverObjectMoveController(Observer *observer, int notificationType)
    {
        assert(sceneDisplayer != nullptr);
        sceneDisplayer->getObjectMoveController()->addObserver(observer, notificationType);
    }

    void SceneDisplayerWidget::onCtrlXPressed()
    {
        if(sceneDisplayer)
        {
            sceneDisplayer->getObjectMoveController()->onCtrlXYZ(0);
        }
	}

    void SceneDisplayerWidget::onCtrlYPressed()
    {
        if(sceneDisplayer)
        {
            sceneDisplayer->getObjectMoveController()->onCtrlXYZ(1);
        }
    }

    void SceneDisplayerWidget::onCtrlZPressed()
    {
        if(sceneDisplayer)
        {
            sceneDisplayer->getObjectMoveController()->onCtrlXYZ(2);
        }
    }

}
