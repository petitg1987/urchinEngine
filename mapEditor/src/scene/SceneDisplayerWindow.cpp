#include <utility>
#include <QtWidgets/QShortcut>

#include "SceneDisplayerWindow.h"
#include "SceneWindowController.h"
#include <widget/controller/mouse/MouseController.h>

#define PICKING_RAY_LENGTH 100.0f

namespace urchin {

    SceneDisplayerWindow::SceneDisplayerWindow(QWidget* parent, const std::unique_ptr<StatusBarController>& statusBarController, std::string mapEditorPath) :
            statusBarController(statusBarController),
            mapEditorPath(std::move(mapEditorPath)),
            sceneWindowController(std::make_unique<SceneWindowController>(this)),
            mouseController(std::make_unique<MouseController>(this)),
            sceneDisplayer(nullptr),
            viewProperties(),
            mouseX(0),
            mouseY(0) {
        GraphicService::enableUniqueSurface();
        setSurfaceType(QSurface::VulkanSurface);

        QObject::connect(new QShortcut(QKeySequence((int)Qt::CTRL + Qt::Key_X), parent), SIGNAL(activated()), this, SLOT(onCtrlXPressed()));
        QObject::connect(new QShortcut(QKeySequence((int)Qt::CTRL + Qt::Key_Y), parent), SIGNAL(activated()), this, SLOT(onCtrlYPressed()));
        QObject::connect(new QShortcut(QKeySequence((int)Qt::CTRL + Qt::Key_Z), parent), SIGNAL(activated()), this, SLOT(onCtrlZPressed()));
    }

    SceneDisplayerWindow::~SceneDisplayerWindow() {
        delete sceneDisplayer;
        GraphicService::destroySurface();
    }

    void SceneDisplayerWindow::exposeEvent(QExposeEvent *) {
        if (isExposed()) {
            render(); //launch first frame
        }
    }

    bool SceneDisplayerWindow::event(QEvent *e) {
        switch (e->type()) {
            case QEvent::UpdateRequest:
                render();
                break;
            case QEvent::Resize:
                if (sceneDisplayer) {
                    int width = 0, height = 0;
                    SceneWindowController(this).getFramebufferSizeRetriever()->getFramebufferSizeInPixel(width, height);
                    sceneDisplayer->resize((unsigned int)width, (unsigned int)height);
                }
                break;
            case QEvent::Leave: //mouse leaves widget
                if (sceneDisplayer && sceneDisplayer->getObjectMoveController() && !geometry().contains(mapFromGlobal(QCursor::pos()))) {
                    sceneDisplayer->getObjectMoveController()->onMouseOut();
                }
                break;
            default:
                break;
        }
        return QWindow::event(e);
    }

    void SceneDisplayerWindow::setupVkInstance(VkInstance instance) {
        vulkanInstance.setVkInstance(instance);
        if (!vulkanInstance.create()) {
            throw std::runtime_error("Failed to create Vulkan instance: " + std::to_string(vulkanInstance.errorCode()));
        }
        setVulkanInstance(&vulkanInstance);
    }

    void SceneDisplayerWindow::clearVkInstance() {
        vulkanInstance.destroy();
        setVulkanInstance(nullptr);
    }

    void SceneDisplayerWindow::loadMap(SceneController* sceneController, const std::string& mapFilename, const std::string& relativeWorkingDirectory) {
        closeMap();
        statusBarController->applyState(StatusBarState::MAP_LOADED);

        sceneDisplayer = new SceneDisplayer(sceneWindowController, sceneController, mouseController, statusBarController);
        sceneDisplayer->loadMap(mapEditorPath, mapFilename, relativeWorkingDirectory);
        sceneDisplayer->resize((unsigned int)geometry().width(), (unsigned int)geometry().height());
        sceneController->setup(sceneDisplayer->getMapHandler());
        updateSceneDisplayerViewProperties();
    }

    void SceneDisplayerWindow::loadEmptyScene() {
        sceneDisplayer = new SceneDisplayer(sceneWindowController, nullptr, mouseController, statusBarController);
        sceneDisplayer->loadEmptyScene(mapEditorPath);
        sceneDisplayer->resize((unsigned int)geometry().width(), (unsigned int)geometry().height());
    }

    void SceneDisplayerWindow::saveState(const std::string& mapFilename) const {
        if (sceneDisplayer && sceneDisplayer->getCamera()) {
            sceneDisplayer->getCamera()->saveCameraState(mapFilename);
        }
    }

    void SceneDisplayerWindow::closeMap() {
        statusBarController->clearState();
        clearVkInstance();

        delete sceneDisplayer;
        sceneDisplayer = nullptr;
    }

    void SceneDisplayerWindow::setViewProperties(SceneDisplayer::ViewProperties viewProperty, bool value) {
        viewProperties[viewProperty] = value;
        updateSceneDisplayerViewProperties();
    }

    void SceneDisplayerWindow::setHighlightSceneObject(const SceneObject* highlightSceneObject) {
        if (sceneDisplayer) {
            sceneDisplayer->setHighlightSceneObject(highlightSceneObject);
        }
    }

    void SceneDisplayerWindow::setHighlightCompoundShapeComponent(std::shared_ptr<const LocalizedCollisionShape> selectedCompoundShapeComponent) {
        if (sceneDisplayer) {
            sceneDisplayer->getBodyShapeDisplayer()->setSelectedCompoundShapeComponent(std::move(selectedCompoundShapeComponent));
        }
    }

    void SceneDisplayerWindow::setHighlightSceneLight(const SceneLight* highlightSceneLight) {
        if (sceneDisplayer) {
            sceneDisplayer->setHighlightSceneLight(highlightSceneLight);
        }
    }

    void SceneDisplayerWindow::setHighlightSceneSound(const SceneSound* highlightSceneSound) {
        if (sceneDisplayer) {
            sceneDisplayer->setHighlightSceneSound(highlightSceneSound);
        }
    }

    void SceneDisplayerWindow::updateSceneDisplayerViewProperties() {
        if (sceneDisplayer) {
            for (unsigned int i = 0; i < SceneDisplayer::LAST_VIEW_PROPERTIES; ++i) {
                sceneDisplayer->setViewProperties(static_cast<SceneDisplayer::ViewProperties>(i), viewProperties[i]);
            }
        }
    }

    void SceneDisplayerWindow::render() {
        if (!sceneDisplayer) {
            loadEmptyScene();
        }

        sceneDisplayer->paint();

        requestUpdate();
    }

    void SceneDisplayerWindow::keyPressEvent(QKeyEvent* event) {
        if (sceneDisplayer) {
            if (event->key() < 256) {
                sceneDisplayer->getSceneManager()->onKeyPress((unsigned int)event->key());
                sceneDisplayer->getSceneManager()->onChar((unsigned int)event->text().toLatin1()[0]);
            } else if (event->key() == Qt::Key_Left) {
                sceneDisplayer->getSceneManager()->onKeyPress(InputDeviceKey::LEFT_ARROW);
            } else if (event->key() == Qt::Key_Right) {
                sceneDisplayer->getSceneManager()->onKeyPress(InputDeviceKey::RIGHT_ARROW);
            } else if (event->key() == Qt::Key_Backspace) {
                sceneDisplayer->getSceneManager()->onChar(8);
            } else if (event->key() == Qt::Key_Delete) {
                sceneDisplayer->getSceneManager()->onChar(127);
            }
        }
    }

    void SceneDisplayerWindow::keyReleaseEvent(QKeyEvent* event) {
        if (sceneDisplayer) {
            if (event->key() < 256) {
                sceneDisplayer->getSceneManager()->onKeyRelease((unsigned int)event->key());
            } else if (event->key() == Qt::Key_Left) {
                sceneDisplayer->getSceneManager()->onKeyRelease(InputDeviceKey::LEFT_ARROW);
            } else if (event->key() == Qt::Key_Right) {
                sceneDisplayer->getSceneManager()->onKeyRelease(InputDeviceKey::RIGHT_ARROW);
            } else if (event->key() == Qt::Key_Escape) {
                sceneDisplayer->getObjectMoveController()->onEscapeKey();
            }
        }
    }

    void SceneDisplayerWindow::mousePressEvent(QMouseEvent* event) {
        if (sceneDisplayer) {
            if (event->buttons() == Qt::LeftButton) {
                sceneDisplayer->getSceneManager()->onKeyPress(InputDeviceKey::MOUSE_LEFT);
            } else if (event->button() == Qt::RightButton) {
                sceneDisplayer->getSceneManager()->onKeyPress(InputDeviceKey::MOUSE_RIGHT);
            }
        }
    }

    void SceneDisplayerWindow::mouseReleaseEvent(QMouseEvent* event) {
        if (sceneDisplayer) {
            if (event->button() == Qt::LeftButton) {
                bool propagateEvent = sceneDisplayer->getObjectMoveController() == nullptr || sceneDisplayer->getObjectMoveController()->onMouseLeftButton();
                if (propagateEvent) {
                    propagateEvent = onMouseClickBodyPickup();
                    if (propagateEvent) {
                        sceneDisplayer->getSceneManager()->onKeyRelease(InputDeviceKey::MOUSE_LEFT);
                    }
                }
            } else if (event->button() == Qt::RightButton) {
                sceneDisplayer->getSceneManager()->onKeyRelease(InputDeviceKey::MOUSE_RIGHT);
            }
        }
    }

    void SceneDisplayerWindow::mouseMoveEvent(QMouseEvent* event) {
        this->mouseX = event->x();
        this->mouseY = event->y();

        if (sceneDisplayer) {
            bool propagateEvent = sceneDisplayer->getSceneManager()->onMouseMove(mouseX, mouseY);
            if (propagateEvent && sceneDisplayer->getObjectMoveController()) {
                sceneDisplayer->getObjectMoveController()->onMouseMove(mouseX, mouseY);
            }
        }
    }

    bool SceneDisplayerWindow::onMouseClickBodyPickup() {
        bool propagateEvent = true;

        if (sceneDisplayer->getSceneManager()->getActiveRenderer3d()) {
            Camera *camera = sceneDisplayer->getSceneManager()->getActiveRenderer3d()->getCamera();
            Ray<float> pickingRay = CameraSpaceService(camera).screenPointToRay(Point2<float>((float) mouseX, (float) mouseY), PICKING_RAY_LENGTH);
            std::shared_ptr<const RayTestResult> rayTestResult = sceneDisplayer->getPhysicsWorld()->rayTest(pickingRay);

            while (!rayTestResult->isResultReady()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            const ccd_set &pickedObjects = rayTestResult->getResults();
            if (!pickedObjects.empty()) {
                lastPickedBodyId = (*rayTestResult->getResults().begin())->getBody2()->getId();
                notifyObservers(this, BODY_PICKED);
                propagateEvent = false;
            } else {
                lastPickedBodyId = "";
                notifyObservers(this, BODY_PICKED);
            }
        }

        return propagateEvent;
    }

    const std::string& SceneDisplayerWindow::getLastPickedBodyId() const {
        return lastPickedBodyId;
    }

    void SceneDisplayerWindow::addObserverObjectMoveController(Observer* observer, int notificationType) {
        assert(sceneDisplayer != nullptr);
        sceneDisplayer->getObjectMoveController()->addObserver(observer, notificationType);
    }

    void SceneDisplayerWindow::onCtrlXPressed() {
        if (sceneDisplayer && sceneDisplayer->getObjectMoveController()) {
            sceneDisplayer->getObjectMoveController()->onCtrlXYZ(0);
        }
    }

    void SceneDisplayerWindow::onCtrlYPressed() {
        if (sceneDisplayer && sceneDisplayer->getObjectMoveController()) {
            sceneDisplayer->getObjectMoveController()->onCtrlXYZ(1);
        }
    }

    void SceneDisplayerWindow::onCtrlZPressed() {
        if (sceneDisplayer && sceneDisplayer->getObjectMoveController()) {
            sceneDisplayer->getObjectMoveController()->onCtrlXYZ(2);
        }
    }

}
