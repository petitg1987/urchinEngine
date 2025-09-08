#include <utility>
#include <QtWidgets/QShortcut>

#include "scene/SceneDisplayerWindow.h"
#include "widget/controller/mouse/MouseController.h"

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
        setSurfaceType(VulkanSurface);

        connect(new QShortcut(QKeySequence((int)Qt::CTRL + Qt::Key_X), parent), &QShortcut::activated, this, [this]() {onCtrlXYZPressed(0);});
        connect(new QShortcut(QKeySequence((int)Qt::CTRL + Qt::Key_Y), parent), &QShortcut::activated, this, [this]() {onCtrlXYZPressed(1);});
        connect(new QShortcut(QKeySequence((int)Qt::CTRL + Qt::Key_Z), parent), &QShortcut::activated, this, [this]() {onCtrlXYZPressed(2);});
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
                    if (width != 0 && height != 0) {
                        sceneDisplayer->resize(width, height);
                    }
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
        statusBarController.applyState(MAP_LOADED);

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

    void SceneDisplayerWindow::setHighlightObjectPhysicsShapes(std::vector<const ObjectEntity*> highlightObjectEntities) const {
        if (sceneDisplayer) {
            sceneDisplayer->setHighlightObjectPhysicsShapes(std::move(highlightObjectEntities));
        }
    }

    void SceneDisplayerWindow::setHighlightCompoundShapeComponent(const LocalizedCollisionShape* selectedCompoundShapeComponent) const {
        if (sceneDisplayer) {
            sceneDisplayer->getBodyShapeDisplayer()->setSelectedCompoundShapeComponent(selectedCompoundShapeComponent);
        }
    }

    void SceneDisplayerWindow::setHighlightObjectLights(std::vector<const ObjectEntity*> highlightObjectEntities) const {
        if (sceneDisplayer) {
            sceneDisplayer->setHighlightObjectLights(std::move(highlightObjectEntities));
        }
    }

    void SceneDisplayerWindow::setHighlightObjectSounds(std::vector<const ObjectEntity*> highlightObjectEntities) const {
        if (sceneDisplayer) {
            sceneDisplayer->setHighlightObjectSounds(std::move(highlightObjectEntities));
        }
    }

    void SceneDisplayerWindow::updateSceneDisplayerViewProperties() const {
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
            sceneDisplayer->getCamera()->moveForward(sceneDisplayer->getScene().getDeltaTimeInSec());
        }
        if (backwardKeyPressed) {
            sceneDisplayer->getCamera()->moveBackward(sceneDisplayer->getScene().getDeltaTimeInSec());
        }
        if (leftKeyPressed) {
            sceneDisplayer->getCamera()->moveLeft(sceneDisplayer->getScene().getDeltaTimeInSec());
        }
        if (rightKeyPressed) {
            sceneDisplayer->getCamera()->moveRight(sceneDisplayer->getScene().getDeltaTimeInSec());
        }

        sceneDisplayer->paint();

        requestUpdate();
    }

    void SceneDisplayerWindow::keyPressEvent(QKeyEvent* event) {
        if (sceneDisplayer) {
            bool propagatePressKeyEvent = true;

            //engine
            if (!event->text().isEmpty()) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onChar((unsigned int)event->text().toLatin1()[0]);
            }
            if (propagatePressKeyEvent) {
                if (event->key() == Qt::Key_A) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::A);
                } else if (event->key() == Qt::Key_C) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::C);
                } else if (event->key() == Qt::Key_V) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::V);
                } else if (event->key() == Qt::Key_X) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::X);
                } else if (event->key() == Qt::Key_Control) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::CTRL);
                } else if (event->key() == Qt::Key_Left) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::LEFT_ARROW);
                } else if (event->key() == Qt::Key_Right) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::RIGHT_ARROW);
                } else if (event->key() == Qt::Key_Up) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::UP_ARROW);
                } else if (event->key() == Qt::Key_Down) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::DOWN_ARROW);
                } else if (event->key() == Qt::Key_Backspace) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::BACKSPACE);
                } else if (event->key() == Qt::Key_Delete) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::DELETE_KEY);
                } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
                    propagatePressKeyEvent = sceneDisplayer->getScene().onKeyPress(InputDeviceKey::ENTER);
                }
            }

            //map editor
            if (propagatePressKeyEvent) {
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
    }

    void SceneDisplayerWindow::keyReleaseEvent(QKeyEvent* event) {
        if (sceneDisplayer) {
            bool propagatePressKeyEvent = true;

            //engine
            if (event->key() == Qt::Key_A) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::A);
            } else if (event->key() == Qt::Key_C) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::C);
            } else if (event->key() == Qt::Key_V) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::V);
            } else if (event->key() == Qt::Key_X) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::X);
            } else if (event->key() == Qt::Key_Control) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::CTRL);
            } else if (event->key() == Qt::Key_Left) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::LEFT_ARROW);
            } else if (event->key() == Qt::Key_Right) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::RIGHT_ARROW);
            } else if (event->key() == Qt::Key_Up) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::UP_ARROW);
            } else if (event->key() == Qt::Key_Down) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::DOWN_ARROW);
            } else if (event->key() == Qt::Key_Backspace) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::BACKSPACE);
            } else if (event->key() == Qt::Key_Delete) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::DELETE_KEY);
            } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::ENTER);
            }

            //map editor
            if (propagatePressKeyEvent) {
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
    }

    void SceneDisplayerWindow::mousePressEvent(QMouseEvent* event) {
        if (sceneDisplayer) {
            //engine
            if (event->button() == Qt::LeftButton) {
                sceneDisplayer->getScene().onKeyPress(InputDeviceKey::MOUSE_LEFT);
            } else if (event->button() == Qt::RightButton) {
                sceneDisplayer->getScene().onKeyPress(InputDeviceKey::MOUSE_RIGHT);
            }
        }
    }

    void SceneDisplayerWindow::mouseReleaseEvent(QMouseEvent* event) {
        if (sceneDisplayer) {
            bool propagatePressKeyEvent = true;

            //engine
            if (event->button() == Qt::LeftButton) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::MOUSE_LEFT);
            } else if (event->button() == Qt::RightButton) {
                propagatePressKeyEvent = sceneDisplayer->getScene().onKeyRelease(InputDeviceKey::MOUSE_RIGHT);
            }

            //map editor
            if (propagatePressKeyEvent) {
                if (event->button() == Qt::LeftButton) {
                    if (sceneDisplayer->getObjectMoveController()) {
                        propagatePressKeyEvent = sceneDisplayer->getObjectMoveController()->onMouseLeftButton();
                    }
                    if (propagatePressKeyEvent) {
                        onMouseClickBodyPickup();
                    }
                }
            }
        }
    }

    void SceneDisplayerWindow::mouseMoveEvent(QMouseEvent* event) {
        //mouse coordinate computed in pixel
        double newMouseX = (double)event->x() * devicePixelRatio();
        double newMouseY = (double)event->y() * devicePixelRatio();

        double deltaMouseX = newMouseX - mouseX;
        double deltaMouseY = newMouseY - mouseY;

        this->mouseX = newMouseX;
        this->mouseY = newMouseY;

        if (sceneDisplayer) {
            bool propagateEvent = sceneDisplayer->getScene().onMouseMove(mouseX, mouseY, deltaMouseX, deltaMouseY);
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
            Ray<float> pickingRay = CameraSpaceService(camera).screenPointToRay(Point2((float) mouseX, (float) mouseY), PICKING_RAY_LENGTH);
            std::shared_ptr<RayTester> rayTester = RayTester::newRayTester();
            sceneDisplayer->getPhysicsWorld().triggerRayTest(rayTester, pickingRay);

            while (!rayTester->isResultReady()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
            }
            std::optional<ContinuousCollisionResult<float>> pickedModels = rayTester->getNearestResult();
            if (pickedModels.has_value()) {
                lastPickedBodyId = pickedModels->getBody2().getId();
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

    void SceneDisplayerWindow::onCtrlXYZPressed(int axisIndex) {
        requestActivate(); //requested to ensure next keys pressed (e.g.: escape) works correctly
        if (sceneDisplayer && sceneDisplayer->getObjectMoveController()) {
            sceneDisplayer->getObjectMoveController()->onCtrlXYZ(axisIndex);
        }
    }

}
