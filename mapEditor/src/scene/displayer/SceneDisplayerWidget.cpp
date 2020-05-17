#include <GL/glew.h>
#include <utility>

#include "SceneDisplayerWidget.h"

#define PICKING_RAY_LENGTH 100.0f

namespace urchin
{

	SceneDisplayerWidget::SceneDisplayerWidget(QWidget *parent, std::string mapEditorPath) :
			QGLWidget(parent),
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
	}

	SceneDisplayerWidget::~SceneDisplayerWidget()
	{
	    if(context()->isValid())
        {
            context()->doneCurrent();
        }

		delete sceneDisplayer;
	}

	MapHandler *SceneDisplayerWidget::newMap(const std::string &mapFilename, const std::string &relativeWorkingDirectory)
	{
		closeMap();

		sceneDisplayer = new SceneDisplayer(this);
		sceneDisplayer->initializeFromNewMap(mapEditorPath, mapFilename, relativeWorkingDirectory);
		sceneDisplayer->resize(static_cast<unsigned int>(geometry().width()), static_cast<unsigned int>(geometry().height()));
		updateSceneDisplayerViewProperties();

		return sceneDisplayer->getMapHandler();
	}

	MapHandler *SceneDisplayerWidget::openMap(const std::string &mapFilename)
	{
		closeMap();

		sceneDisplayer = new SceneDisplayer(this);
		sceneDisplayer->initializeFromExistingMap(mapEditorPath, mapFilename);
		sceneDisplayer->resize(static_cast<unsigned int>(geometry().width()), static_cast<unsigned int>(geometry().height()));
		updateSceneDisplayerViewProperties();

		return sceneDisplayer->getMapHandler();
	}

	void SceneDisplayerWidget::saveMap(const std::string &mapFilename) const
	{
		if(sceneDisplayer)
        {
		    sceneDisplayer->getCamera()->saveCameraState(mapFilename);
        }
	}

	void SceneDisplayerWidget::closeMap()
	{
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
			sceneDisplayer->setHighlightCompoundShapeComponent(std::move(selectedCompoundShapeComponent));
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
		if(!sceneDisplayer)
		{
			return;
		}

		if(event->key() < 256)
		{
			sceneDisplayer->getSceneManager()->onKeyDown(static_cast<unsigned int>(event->key()));
			sceneDisplayer->getSceneManager()->onChar(static_cast<unsigned int>(event->text().toLatin1()[0]));
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
		if(!sceneDisplayer)
		{
			return;
		}

		if(event->key() < 256)
		{
			sceneDisplayer->getSceneManager()->onKeyUp(static_cast<unsigned int>(event->key()));
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
		if(!sceneDisplayer)
		{
			return;
		}

		if(event->buttons() == Qt::LeftButton)
		{
			sceneDisplayer->getSceneManager()->onKeyDown(InputDevice::Key::MOUSE_LEFT);

            Camera *camera = sceneDisplayer->getSceneManager()->getActiveRenderer3d()->getCamera();
            float clipSpaceX = (2.0f * (float)mouseX) / ((float)sceneDisplayer->getSceneManager()->getSceneWidth()) - 1.0f;
            float clipSpaceY = 1.0f - (2.0f * (float)mouseY) / ((float)sceneDisplayer->getSceneManager()->getSceneHeight());
            urchin::Vector4<float> rayDirectionClipSpace(clipSpaceX, clipSpaceY, -1.0f, 1.0f);
            urchin::Vector4<float> rayDirectionEyeSpace = camera->getProjectionMatrix().inverse() * rayDirectionClipSpace;
            rayDirectionEyeSpace.setValues(rayDirectionEyeSpace.X, rayDirectionEyeSpace.Y, -1.0f, 0.0f);
            urchin::Vector3<float> rayDirectionWorldSpace = (camera->getViewMatrix().inverse() * rayDirectionEyeSpace).xyz().normalize();

            const urchin::Point3<float> &rayStart = camera->getPosition();
            urchin::Point3<float> rayEnd = rayStart.translate(rayDirectionWorldSpace * PICKING_RAY_LENGTH);
            Ray<float> pickingRay(rayStart, rayEnd);
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
            }
		}else if (event->button() == Qt::RightButton)
		{
			sceneDisplayer->getSceneManager()->onKeyDown(InputDevice::Key::MOUSE_RIGHT);
		}
	}

	void SceneDisplayerWidget::mouseReleaseEvent(QMouseEvent *event)
	{
		if(!sceneDisplayer)
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
        this->mouseX = event->x();
        this->mouseY = event->y();

		if(sceneDisplayer)
		{
			sceneDisplayer->getSceneManager()->onMouseMove(mouseX, mouseY);
		}
	}

	const std::string &SceneDisplayerWidget::getLastPickedBodyId() const
	{
		return lastPickedBodyId;
	}

}
