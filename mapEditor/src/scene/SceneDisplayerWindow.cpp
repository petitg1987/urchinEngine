#include <utility>
#include <QtWidgets/QShortcut>

#include <scene/SceneDisplayerWindow.h>
#include <widget/controller/mouse/MouseController.h>

namespace urchin {

    SceneDisplayerWindow::SceneDisplayerWindow(QWidget* parent, StatusBarController& statusBarController, std::string mapEditorPath) :
            statusBarController(statusBarController),
            mapEditorPath(std::move(mapEditorPath)),
            sceneWindowController(SceneWindowController(this)),
            mouseController(MouseController(this)),
            viewProperties(),
            forwardKeyPressed(false),
            backwardKeyPressed(false),
            leftKeyPressed(false),
            rightKeyPressed(false),
            mouseX(0),
            mouseY(0) {
        GraphicsApiService::enableUniqueSurface();
        setSurfaceType(QSurface::VulkanSurface);

        QObject::connect(new QShortcut(QKeySequence((int)Qt::CTRL + Qt::Key_X), parent), SIGNAL(activated()), this, SLOT(onCtrlXPressed()));
        QObject::connect(new QShortcut(QKeySequence((int)Qt::CTRL + Qt::Key_Y), parent), SIGNAL(activated()), this, SLOT(onCtrlYPressed()));
        QObject::connect(new QShortcut(QKeySequence((int)Qt::CTRL + Qt::Key_Z), parent), SIGNAL(activated()), this, SLOT(onCtrlZPressed()));
    }

    SceneDisplayerWindow::~SceneDisplayerWindow() {
        GraphicsApiService::destroySurface();
    }

    void SceneDisplayerWindow::exposeEvent(QExposeEvent *) {
        if (isExposed()) {
            render(); //launch first frame
        }
    }

    bool SceneDisplayerWindow::event(QEvent *e) {
        switch (e->type()) {
            case QEvent::UpdateRequest: {
                render();
                break;
            } case QEvent::Resize: {
                if (sceneDisplayer) {
                    unsigned int width = 0;
                    unsigned int height = 0;
                    SceneWindowController(this).getFramebufferSizeRetriever()->getFramebufferSizeInPixel(width, height);
                    sceneDisplayer->resize(width, height);
                }
                break;
            } case QEvent::Leave: { //mouse leaves widget
                Point2<double> mousePositionInPixel = mouseController.getMousePosition();
                if (sceneDisplayer && sceneDisplayer->getObjectMoveController() && !geometry().contains(QPoint((int)mousePositionInPixel.X, (int)mousePositionInPixel.Y))) {
                    sceneDisplayer->getObjectMoveController()->onMouseOut();
                }
                break;
            } default: {
                break;
            }
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

    void SceneDisplayerWindow::loadMap(SceneController& sceneController, const std::string& mapFilename, const std::string& relativeWorkingDirectory) {
        closeMap();
        statusBarController.applyState(StatusBarState::MAP_LOADED);

        sceneDisplayer = std::make_unique<SceneDisplayer>(sceneWindowController, &sceneController, mouseController, statusBarController);
        sceneDisplayer->loadMap(mapEditorPath, mapFilename, relativeWorkingDirectory);
        sceneDisplayer->resize( //size is computed in pixel
                MathFunction::roundToUInt((float)geometry().width() * (float)devicePixelRatio()),
                MathFunction::roundToUInt((float)geometry().height() * (float)devicePixelRatio()));
        sceneController.setup(sceneDisplayer->getMap());
        updateSceneDisplayerViewProperties();
    }

    void SceneDisplayerWindow::loadEmptyScene() {
        sceneDisplayer = std::make_unique<SceneDisplayer>(sceneWindowController, nullptr, mouseController, statusBarController);
        sceneDisplayer->loadEmptyScene(mapEditorPath);
        sceneDisplayer->resize( //size is computed in pixel
                MathFunction::roundToUInt((float)geometry().width() * (float)devicePixelRatio()),
                MathFunction::roundToUInt((float)geometry().height() * (float)devicePixelRatio()));
    }

    void SceneDisplayerWindow::saveState(const std::string& mapFilename) const {
        if (sceneDisplayer && sceneDisplayer->getCamera()) {
            sceneDisplayer->getCamera()->saveCameraState(mapFilename);
        }
    }

    void SceneDisplayerWindow::closeMap() {
        statusBarController.clearState();
        clearVkInstance();

        sceneDisplayer.reset(nullptr);
    }

    void SceneDisplayerWindow::setViewProperties(SceneDisplayer::ViewProperties viewProperty, bool value) {
        viewProperties[viewProperty] = value;
        updateSceneDisplayerViewProperties();
    }

    void SceneDisplayerWindow::setHighlightObjectEntity(const ObjectEntity* highlightObjectEntity) const {
        if (sceneDisplayer) {
            sceneDisplayer->setHighlightObjectEntity(highlightObjectEntity);
        }
    }

    void SceneDisplayerWindow::setHighlightCompoundShapeComponent(const LocalizedCollisionShape* selectedCompoundShapeComponent) const {
        if (sceneDisplayer) {
            sceneDisplayer->getBodyShapeDisplayer()->setSelectedCompoundShapeComponent(selectedCompoundShapeComponent);
        }
    }

    void SceneDisplayerWindow::setHighlightLightEntity(const LightEntity* highlightLightEntity) const {
        if (sceneDisplayer) {
            sceneDisplayer->setHighlightLightEntity(highlightLightEntity);
        }
    }

    void SceneDisplayerWindow::setHighlightSoundEntity(const SoundEntity* highlightSoundEntity) const {
        if (sceneDisplayer) {
            sceneDisplayer->setHighlightSoundEntity(highlightSoundEntity);
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

        if (forwardKeyPressed) {
            sceneDisplayer->getCamera()->moveForward(sceneDisplayer->getScene().getDeltaTime());
        }
        if (backwardKeyPressed) {
            sceneDisplayer->getCamera()->moveBackward(sceneDisplayer->getScene().getDeltaTime());
        }
        if (leftKeyPressed) {
            sceneDisplayer->getCamera()->moveLeft(sceneDisplayer->getScene().getDeltaTime());
        }
        if (rightKeyPressed) {
            sceneDisplayer->getCamera()->moveRight(sceneDisplayer->getScene().getDeltaTime());
        }

        sceneDisplayer->paint();

        requestUpdate();
    }

    void SceneDisplayerWindow::keyPressEvent(QKeyEvent* event) {
        if (sceneDisplayer) {
            //engine
            if (event->key() < 256) {
                sceneDisplayer->getScene().onKeyPress((unsigned int)event->key()); //TODO call method for A-Z + take argument method InputDeviceKey + adapt GLFW
                sceneDisplayer->getScene().onChar((unsigned int)event->text().toLatin1()[0]);
            } else if (event->key() == Qt::Key_Control) {
                sceneDisplayer->getScene().onKeyPress((unsigned int)InputDeviceKey::CTRL);
            } else if (event->key() == Qt::Key_Left) {
                sceneDisplayer->getScene().onKeyPress((unsigned int)InputDeviceKey::LEFT_ARROW);
            } else if (event->key() == Qt::Key_Right) {
                sceneDisplayer->getScene().onKeyPress((unsigned int)InputDeviceKey::RIGHT_ARROW);
            } else if (event->key() == Qt::Key_Up) {
                sceneDisplayer->getScene().onKeyPress((unsigned int)InputDeviceKey::UP_ARROW);
            } else if (event->key() == Qt::Key_Down) {
                sceneDisplayer->getScene().onKeyPress((unsigned int)InputDeviceKey::DOWN_ARROW);
            } else if (event->key() == Qt::Key_Backspace) {
                sceneDisplayer->getScene().onKeyPress((unsigned int)InputDeviceKey::BACKSPACE);
            } else if (event->key() == Qt::Key_Delete) {
                sceneDisplayer->getScene().onKeyPress((unsigned int)InputDeviceKey::DELETE_KEY);
            } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
                sceneDisplayer->getScene().onKeyPress((unsigned int)InputDeviceKey::ENTER);
            }

            //map editor
            if (event->key() == Qt::Key_Z) {
                forwardKeyPressed = true;
            } else if (event->key() == Qt::Key_S) {
                backwardKeyPressed = true;
            } else if (event->key() == Qt::Key_Q) {
                leftKeyPressed = true;
            } else if (event->key() == Qt::Key_D) {
                rightKeyPressed = true;
            }
        }
    }

    void SceneDisplayerWindow::keyReleaseEvent(QKeyEvent* event) {
        if (sceneDisplayer) {
            //engine
            if (event->key() < 256) {
                sceneDisplayer->getScene().onKeyRelease((unsigned int)event->key());
            } else if (event->key() == Qt::Key_Control) {
                sceneDisplayer->getScene().onKeyRelease((unsigned int)InputDeviceKey::CTRL);
            } else if (event->key() == Qt::Key_Left) {
                sceneDisplayer->getScene().onKeyRelease((unsigned int)InputDeviceKey::LEFT_ARROW);
            } else if (event->key() == Qt::Key_Right) {
                sceneDisplayer->getScene().onKeyRelease((unsigned int)InputDeviceKey::RIGHT_ARROW);
            } else if (event->key() == Qt::Key_Up) {
                sceneDisplayer->getScene().onKeyRelease((unsigned int)InputDeviceKey::UP_ARROW);
            } else if (event->key() == Qt::Key_Down) {
                sceneDisplayer->getScene().onKeyRelease((unsigned int)InputDeviceKey::DOWN_ARROW);
            } else if (event->key() == Qt::Key_Backspace) {
                sceneDisplayer->getScene().onKeyRelease((unsigned int)InputDeviceKey::BACKSPACE);
            } else if (event->key() == Qt::Key_Delete) {
                sceneDisplayer->getScene().onKeyRelease((unsigned int)InputDeviceKey::DELETE_KEY);
            } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
                sceneDisplayer->getScene().onKeyRelease((unsigned int)InputDeviceKey::ENTER);
            }

            //map editor
            if (event->key() == Qt::Key_Z) {
                forwardKeyPressed = false;
            } else if (event->key() == Qt::Key_S) {
                backwardKeyPressed = false;
            } else if (event->key() == Qt::Key_Q) {
                leftKeyPressed = false;
            } else if (event->key() == Qt::Key_D) {
                rightKeyPressed = false;
            } else if (event->key() == Qt::Key_Escape) {
                sceneDisplayer->getObjectMoveController()->onEscapeKey();
            }
        }
    }

    void SceneDisplayerWindow::mousePressEvent(QMouseEvent* event) {
        if (sceneDisplayer) {
            //engine
            if (event->buttons() == Qt::LeftButton) {
                sceneDisplayer->getScene().onKeyPress((unsigned int)InputDeviceKey::MOUSE_LEFT);
            } else if (event->button() == Qt::RightButton) {
                sceneDisplayer->getScene().onKeyPress((unsigned int)InputDeviceKey::MOUSE_RIGHT);
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
                        sceneDisplayer->getScene().onKeyRelease((unsigned int)InputDeviceKey::MOUSE_LEFT);
                    }
                }
            } else if (event->button() == Qt::RightButton) {
                sceneDisplayer->getScene().onKeyRelease((unsigned int)InputDeviceKey::MOUSE_RIGHT);
            }
        }
    }

    void SceneDisplayerWindow::mouseMoveEvent(QMouseEvent* event) {
        //mouse coordinate computed in pixel
        this->mouseX = (double)event->x() * devicePixelRatio();
        this->mouseY = (double)event->y() * devicePixelRatio();

        if (sceneDisplayer) {
            bool propagateEvent = sceneDisplayer->getScene().onMouseMove(mouseX, mouseY);
            if (propagateEvent && sceneDisplayer->getObjectMoveController()) {
                sceneDisplayer->getObjectMoveController()->onMouseMove(mouseX, mouseY);
            }
        }
    }

    bool SceneDisplayerWindow::onMouseClickBodyPickup() {
        bool propagateEvent = true;

        if (sceneDisplayer->getScene().getActiveRenderer3d()) {
            constexpr float PICKING_RAY_LENGTH = 100.0f;
            const Camera& camera = sceneDisplayer->getScene().getActiveRenderer3d()->getCamera();
            Ray<float> pickingRay = CameraSpaceService(camera).screenPointToRay(Point2<float>((float) mouseX, (float) mouseY), PICKING_RAY_LENGTH);
            std::shared_ptr<const RayTestResult> rayTestResult = sceneDisplayer->getPhysicsWorld().rayTest(pickingRay);

            while (!rayTestResult->isResultReady()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            const ccd_set& pickedModels = rayTestResult->getResults();
            if (!pickedModels.empty()) {
                lastPickedBodyId = (*pickedModels.begin()).getBody2().getId();
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

    void SceneDisplayerWindow::addObserverObjectMoveController(Observer* observer, int notificationType) const {
        assert(sceneDisplayer);
        sceneDisplayer->getObjectMoveController()->addObserver(observer, notificationType);
    }

    void SceneDisplayerWindow::onCtrlXPressed() const {
        if (sceneDisplayer && sceneDisplayer->getObjectMoveController()) {
            sceneDisplayer->getObjectMoveController()->onCtrlXYZ(0);
        }
    }

    void SceneDisplayerWindow::onCtrlYPressed() const {
        if (sceneDisplayer && sceneDisplayer->getObjectMoveController()) {
            sceneDisplayer->getObjectMoveController()->onCtrlXYZ(1);
        }
    }

    void SceneDisplayerWindow::onCtrlZPressed() const {
        if (sceneDisplayer && sceneDisplayer->getObjectMoveController()) {
            sceneDisplayer->getObjectMoveController()->onCtrlXYZ(2);
        }
    }

}
