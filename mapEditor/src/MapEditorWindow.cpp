#include <stdexcept>
#include <ranges>
#include <QApplication>
#include <QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QFileDialog>
#include <utility>
#include <QtCore/QStandardPaths>

#include <MapEditorWindow.h>
#include <widget/dialog/NewDialog.h>
#include <widget/dialog/NotSavedDialog.h>
#include <panel/objects/ObjectTableView.h>
#include <panel/objects/bodyshape/BodyCompoundShapeWidget.h>
#include <panel/objects/bodyshape/support/LocalizedShapeTableView.h>
#include <panel/lights/LightTableView.h>
#include <panel/sounds/SoundTableView.h>
#include <StateSaveHelper.h>

namespace urchin {

    MapEditorWindow::MapEditorWindow(std::string mapEditorPath) :
            statusBarController(StatusBarController(this)),
            saveAction(nullptr),
            saveAsAction(nullptr),
            closeAction(nullptr),
            reloadAction(nullptr),
            mapEditorPath(std::move(mapEditorPath)),
            sceneController(nullptr),
            sceneDisplayerWindow(nullptr),
            scenePanelWidget(nullptr),
            objectSubTabSelected(0) {
        this->setAttribute(Qt::WA_DeleteOnClose);
        this->setWindowTitle(getBaseWindowTitle());
        this->resize(1200, 675);
        auto* centralWidget = new QWidget(this);

        auto* horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);

        setupMenu();
        statusBarController.clearState();

        setupSceneDisplayerWidget(centralWidget, horizontalLayout);
        setupSceneControllerWidget(centralWidget, horizontalLayout);

        this->setCentralWidget(centralWidget);
    }

    QString MapEditorWindow::getBaseWindowTitle() const {
        return QString::fromStdString("Urchin - Map Editor");
    }

    void MapEditorWindow::setupMenu() {
        auto* menu = new QMenuBar(this);

        auto* fileMenu = new QMenu("File");

        auto* newAction = new QAction("New...", this);
        newAction->setShortcut(QKeySequence("Ctrl+N"));
        fileMenu->addAction(newAction);
        connect(newAction, SIGNAL(triggered()), this, SLOT(showNewDialog()));

        auto* openAction = new QAction("Open...", this);
        openAction->setShortcut(QKeySequence("Ctrl+O"));
        fileMenu->addAction(openAction);
        connect(openAction, SIGNAL(triggered()), this, SLOT(showOpenDialog()));

        saveAction = new QAction("Save", this);
        saveAction->setEnabled(false);
        saveAction->setShortcut(QKeySequence("Ctrl+S"));
        fileMenu->addAction(saveAction);
        connect(saveAction, SIGNAL(triggered()), this, SLOT(executeSaveAction()));

        saveAsAction = new QAction("Save as...", this);
        saveAsAction->setEnabled(false);
        saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
        fileMenu->addAction(saveAsAction);
        connect(saveAsAction, SIGNAL(triggered()), this, SLOT(showSaveAsDialog()));

        closeAction = new QAction("Close", this);
        closeAction->setEnabled(false);
        closeAction->setShortcut(QKeySequence("Ctrl+W"));
        fileMenu->addAction(closeAction);
        connect(closeAction, SIGNAL(triggered()), this, SLOT(executeCloseAction()));

        reloadAction = new QAction("Reload", this);
        reloadAction->setEnabled(false);
        reloadAction->setShortcut(QKeySequence("F5"));
        fileMenu->addAction(reloadAction);
        connect(reloadAction, SIGNAL(triggered()), this, SLOT(executeReloadAction()));

        fileMenu->addSeparator();

        auto* exitAction = new QAction("Exit", this);
        fileMenu->addAction(exitAction);
        connect(exitAction, SIGNAL(triggered()), this, SLOT(executeExitAction()));

        auto* viewMenu = new QMenu("View");

        auto* viewObjectMenu = new QMenu("Object");
        viewMenu->addMenu(viewObjectMenu);
        auto* viewPhysicsShapeAction = new QAction("Physics Shape", this);
        viewPhysicsShapeAction->setEnabled(false);
        viewPhysicsShapeAction->setCheckable(true);
        viewPhysicsShapeAction->setChecked(true);
        viewObjectMenu->addAction(viewPhysicsShapeAction);
        viewActions[SceneDisplayer::OBJECT_SCOPE] = viewPhysicsShapeAction;
        connect(viewPhysicsShapeAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

        auto* viewLightMenu = new QMenu("Light");
        viewMenu->addMenu(viewLightMenu);
        auto* viewLightScopeAction = new QAction("Light Scope", this);
        viewLightScopeAction->setEnabled(false);
        viewLightScopeAction->setCheckable(true);
        viewLightScopeAction->setChecked(true);
        viewLightMenu->addAction(viewLightScopeAction);
        viewActions[SceneDisplayer::LIGHT_SCOPE] = viewLightScopeAction;
        connect(viewLightScopeAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

        auto* viewSoundMenu = new QMenu("Sound");
        viewMenu->addMenu(viewSoundMenu);
        auto* viewSoundScopeAction = new QAction("Sound Scope", this);
        viewSoundScopeAction->setEnabled(false);
        viewSoundScopeAction->setCheckable(true);
        viewSoundScopeAction->setChecked(true);
        viewSoundMenu->addAction(viewSoundScopeAction);
        viewActions[SceneDisplayer::SOUND_SCOPE] = viewSoundScopeAction;
        connect(viewSoundScopeAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

        auto* viewAIMenu = new QMenu("AI");
        viewMenu->addMenu(viewAIMenu);
        auto* viewNavMeshAction = new QAction("Navigation Mesh", this);
        viewNavMeshAction->setEnabled(false);
        viewNavMeshAction->setCheckable(true);
        viewNavMeshAction->setChecked(true);
        viewAIMenu->addAction(viewNavMeshAction);
        viewActions[SceneDisplayer::NAV_MESH] = viewNavMeshAction;
        connect(viewNavMeshAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

        menu->addMenu(fileMenu);
        menu->addMenu(viewMenu);
        this->setMenuBar(menu);
    }

    void MapEditorWindow::setupSceneDisplayerWidget(QWidget* centralWidget, QHBoxLayout* horizontalLayout) {
        sceneDisplayerWindow = new SceneDisplayerWindow(centralWidget, statusBarController, mapEditorPath);
        QWidget* sceneDisplayerWidget = createWindowContainer(sceneDisplayerWindow, centralWidget);

        sceneDisplayerWidget->setMouseTracking(true);
        sceneDisplayerWidget->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sceneDisplayerWidget->sizePolicy().hasHeightForWidth());
        sceneDisplayerWidget->setSizePolicy(sizePolicy);
        sceneDisplayerWidget->show();

        executeViewPropertiesChangeAction();

        horizontalLayout->addWidget(sceneDisplayerWidget);
    }

    void MapEditorWindow::setupSceneControllerWidget(QWidget* centralWidget, QHBoxLayout* horizontalLayout) {
        scenePanelWidget = new ScenePanelWidget(centralWidget);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(scenePanelWidget->sizePolicy().hasHeightForWidth());
        scenePanelWidget->setSizePolicy(sizePolicy);
        scenePanelWidget->setMaximumSize(QSize(380, 16777215));
        scenePanelWidget->getObjectPanelWidget()->addObserver(this, ObjectPanelWidget::OBJECT_BODY_SHAPE_WIDGET_CREATED);
        scenePanelWidget->getObjectPanelWidget()->addObserver(this, ObjectPanelWidget::OBJECT_SUB_TAB_SELECTION_CHANGED);
        scenePanelWidget->getObjectPanelWidget()->getObjectTableView()->addObserver(this, ObjectTableView::OBJECT_SELECTION_CHANGED);
        scenePanelWidget->getLightPanelWidget()->getLightTableView()->addObserver(this, LightTableView::LIGHT_SELECTION_CHANGED);
        scenePanelWidget->getSoundPanelWidget()->getSoundTableView()->addObserver(this, SoundTableView::SOUND_SELECTION_CHANGED);
        scenePanelWidget->addObserver(this, ScenePanelWidget::TAB_SELECTED);
        horizontalLayout->addWidget(scenePanelWidget);

        sceneDisplayerWindow->addObserver(scenePanelWidget->getObjectPanelWidget(), SceneDisplayerWindow::BODY_PICKED);
    }

    QString MapEditorWindow::getPreferredMapPath() const {
        std::string savedPreferredMapPath = StateSaveHelper::instance().retrieveState("preferred.map.path", "./");
        return QString::fromStdString(savedPreferredMapPath);
    }

    void MapEditorWindow::savePreferredMapPath(const std::string& preferredMapPath) const {
        StateSaveHelper::instance().saveState("preferred.map.path", preferredMapPath);
    }

    void MapEditorWindow::notify(Observable* observable, int notificationType) {
        bool refreshObjectHighlight = false;

        if (dynamic_cast<ScenePanelWidget*>(observable)) {
            if (notificationType == ScenePanelWidget::TAB_SELECTED) {
                executeViewPropertiesChangeAction();
            }
        } else if (dynamic_cast<ObjectTableView*>(observable)) {
            if (notificationType == ObjectTableView::OBJECT_SELECTION_CHANGED) {
                refreshObjectHighlight = true;
            }
        } else if (dynamic_cast<AbstractController*>(observable)) {
            if (notificationType == AbstractController::CHANGES_DONE) {
                refreshWindowTitle();
            }
        } else if (const auto* objectPanelWidget = dynamic_cast<ObjectPanelWidget*>(observable)) {
            if (notificationType == ObjectPanelWidget::OBJECT_SUB_TAB_SELECTION_CHANGED) {
                objectSubTabSelected = objectPanelWidget->getTabSelected(); //TODO use enum
                refreshObjectHighlight = true;
            }
        }
        handleCompoundShapeSelectionChange(observable, notificationType);

        if (refreshObjectHighlight) {
            const ObjectEntity* selectedObjectEntity = scenePanelWidget->getObjectPanelWidget()->getObjectTableView()->getSelectedObjectEntity();
            sceneDisplayerWindow->setHighlightObjectMesh(selectedObjectEntity);
            sceneDisplayerWindow->setHighlightObjectLight(objectSubTabSelected == 2 ? selectedObjectEntity : nullptr);
            sceneDisplayerWindow->setHighlightObjectSound(objectSubTabSelected == 3 ? selectedObjectEntity : nullptr);
        }
    }

    void MapEditorWindow::handleCompoundShapeSelectionChange(Observable* observable, int notificationType) {
        if (const auto* objectPanelWidget = dynamic_cast<ObjectPanelWidget*>(observable)) {
            if (notificationType == ObjectPanelWidget::OBJECT_BODY_SHAPE_WIDGET_CREATED) {
                BodyShapeWidget* bodyShapeWidget = objectPanelWidget->getBodyShapeWidget();
                if (const auto* bodyCompoundShapeWidget = dynamic_cast<BodyCompoundShapeWidget*>(bodyShapeWidget)) {
                    bodyCompoundShapeWidget->getLocalizedShapeTableView()->addObserver(this, LocalizedShapeTableView::OBJECT_COMPOUND_SHAPE_SELECTION_CHANGED);
                }
            }
        } else if (const auto* localizedShapeTableView = dynamic_cast<LocalizedShapeTableView*>(observable)) {
            if (notificationType == LocalizedShapeTableView::OBJECT_COMPOUND_SHAPE_SELECTION_CHANGED) {
                sceneDisplayerWindow->setHighlightCompoundShapeComponent(localizedShapeTableView->getSelectedLocalizedShape());
            }
        }
    }

    void MapEditorWindow::showNewDialog() {
        if (checkCurrentMapSaved()) {
            NewDialog newDialog(this);
            newDialog.exec();

            if (newDialog.result() == QDialog::Accepted) {
                loadMap(newDialog.getFilename(), newDialog.getRelativeWorkingDirectory());
                sceneController->forceModified();
            }
        }
    }

    void MapEditorWindow::showOpenDialog() {
        if (checkCurrentMapSaved()) {
            QString filename = QFileDialog::getOpenFileName(this, tr("Open file"), getPreferredMapPath(), "UDA file (*.uda)", nullptr, QFileDialog::DontUseNativeDialog);
            if (!filename.isNull()) {
                std::string mapFilename = filename.toUtf8().constData();
                std::string relativeWorkingDirectory = MapSaveService::getRelativeWorkingDirectory(mapFilename);
                loadMap(mapFilename, relativeWorkingDirectory);
            }
        }
    }

    void MapEditorWindow::loadMap(const std::string& mapFilename, const std::string& relativeWorkingDirectory) {
        sceneController = std::make_unique<SceneController>();

        sceneDisplayerWindow->loadMap(*sceneController, mapFilename, relativeWorkingDirectory);
        scenePanelWidget->loadMap(*sceneController);

        sceneController->addObserverOnAllControllers(this, AbstractController::CHANGES_DONE);
        sceneDisplayerWindow->addObserverObjectMoveController(scenePanelWidget->getObjectPanelWidget(), ObjectMoveController::OBJECT_MOVED);

        updateMapFilename(mapFilename);
        updateInterfaceState();
    }

    void MapEditorWindow::executeSaveAction() {
        sceneDisplayerWindow->saveState(mapFilename);
        if (sceneController) {
            sceneController->saveMapOnFile(mapFilename);
        }

        updateInterfaceState();
    }

    void MapEditorWindow::showSaveAsDialog() {
        QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), getPreferredMapPath(), "UDA file (*.uda)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            std::string filenameString = filename.toUtf8().constData();
            std::string fileExtension = FileUtil::getFileExtension(filenameString);
            if (!StringUtil::insensitiveEquals(fileExtension, ".uda")) {
                filenameString += ".uda";
            }

            sceneDisplayerWindow->saveState(filenameString);
            if (sceneController) {
                sceneController->saveMapOnFile(filenameString);
            }

            updateMapFilename(filenameString);
            updateInterfaceState();
        }
    }

    bool MapEditorWindow::executeCloseAction() {
        bool canProceed = false;
        if (checkCurrentMapSaved()) {
            sceneDisplayerWindow->closeMap();
            scenePanelWidget->closeMap();

            sceneController.reset(nullptr);

            updateMapFilename("");
            updateInterfaceState();

            canProceed = true;
        }

        return canProceed;
    }

    bool MapEditorWindow::executeReloadAction() {
        bool canProceed = false;
        if (checkCurrentMapSaved()) {
            std::string relativeWorkingDirectory = MapSaveService::getRelativeWorkingDirectory(mapFilename);
            loadMap(mapFilename, relativeWorkingDirectory);
            canProceed = true;
        }

        return canProceed;
    }

    void MapEditorWindow::executeExitAction() {
        if (executeCloseAction()) {
            QApplication::quit();
        }
    }

    void MapEditorWindow::closeEvent(QCloseEvent* event) {
        if (executeCloseAction()) {
            close();
            QApplication::quit();
        } else {
            event->ignore();
        }
    }

    bool MapEditorWindow::checkCurrentMapSaved() {
        bool canProceed = true;
        if (sceneController != nullptr && sceneController->isModified()) {
            NotSavedDialog notSavedDialog(this);
            notSavedDialog.exec();

            if (notSavedDialog.result() == QDialog::Accepted && notSavedDialog.needSave()) {
                executeSaveAction();
            } else if (notSavedDialog.result() == QDialog::Rejected) {
                canProceed = false;
            }
        }

        return canProceed;
    }

    void MapEditorWindow::updateInterfaceState() {
        bool hasMapOpen = sceneController != nullptr;

        saveAction->setEnabled(hasMapOpen);
        saveAsAction->setEnabled(hasMapOpen);
        closeAction->setEnabled(hasMapOpen);
        reloadAction->setEnabled(hasMapOpen);
        for (QAction* action : std::views::values(viewActions)) {
            action->setEnabled(hasMapOpen);
        }

        refreshWindowTitle();
    }

    void MapEditorWindow::updateMapFilename(const std::string& mapFilename) {
        this->mapFilename = mapFilename;

        if (!mapFilename.empty()) {
            std::string preferredMapPathString = FileUtil::getDirectory(mapFilename);
            savePreferredMapPath(preferredMapPathString);
        }
    }

    void MapEditorWindow::refreshWindowTitle() {
        if (mapFilename.empty()) {
            this->setWindowTitle(getBaseWindowTitle());
        } else {
            if (sceneController && sceneController->isModified()) {
                this->setWindowTitle(QString::fromStdString("*") + getBaseWindowTitle() + QString::fromStdString(" (" + mapFilename + ")"));
            } else {
                this->setWindowTitle(getBaseWindowTitle() + QString::fromStdString(" (" + mapFilename + ")"));
            }
        }
    }

    void MapEditorWindow::executeViewPropertiesChangeAction() {
        for (int i = 0; i < SceneDisplayer::LAST_VIEW_PROPERTIES; ++i) {
            auto viewProperties = static_cast<SceneDisplayer::ViewProperties>(i);

            bool isViewChecked = viewActions[viewProperties]->isChecked();
            bool isCorrespondingTabSelected = (scenePanelWidget == nullptr && i == 0)
                    || (scenePanelWidget != nullptr && getConcernedTabFor(viewProperties) == scenePanelWidget->getTabSelected());

            sceneDisplayerWindow->setViewProperties(viewProperties, isViewChecked && isCorrespondingTabSelected);
        }
    }

    ScenePanelWidget::TabName MapEditorWindow::getConcernedTabFor(SceneDisplayer::ViewProperties viewProperties) {
        if (SceneDisplayer::OBJECT_SCOPE == viewProperties) {
            return ScenePanelWidget::OBJECTS;
        }
        if (SceneDisplayer::LIGHT_SCOPE == viewProperties) { //TODO remove
            return ScenePanelWidget::LIGHTS;
        }
        if (SceneDisplayer::SOUND_SCOPE == viewProperties) { //TODO remove
            return ScenePanelWidget::SOUNDS;
        }
        if (SceneDisplayer::NAV_MESH == viewProperties) {
            return ScenePanelWidget::AI;
        }

        throw std::runtime_error("Impossible to find concerned tab for properties: " + std::to_string(viewProperties));
    }

}
