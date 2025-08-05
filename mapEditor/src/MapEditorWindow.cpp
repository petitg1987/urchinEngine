#include <stdexcept>
#include <ranges>
#include <QApplication>
#include <QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QFileDialog>
#include <utility>
#include <QtCore/QStandardPaths>

#include "MapEditorWindow.h"
#include "widget/dialog/NewDialog.h"
#include "widget/dialog/NotSavedDialog.h"
#include "widget/dialog/LightMaskNameDialog.h"
#include "panel/object/ObjectTableView.h"
#include "panel/object/physics/bodyshape/BodyCompoundShapeWidget.h"
#include "panel/object/physics/bodyshape/support/LocalizedShapeTableView.h"
#include "StateSaveHelper.h"

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
            objectTabSelected(ObjectPanelWidget::ObjectTab::MODEL) {
        this->setAttribute(Qt::WA_DeleteOnClose);
        this->resize(1200, 675);
        this->showMaximized();
        this->setWindowTitle(getBaseWindowTitle());

        setupMenu();
        statusBarController.clearState();

        auto* centralWidget = new QWidget(this);
        auto* horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);

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
        viewActions[SceneDisplayer::PHYSICS_SHAPE] = viewPhysicsShapeAction;
        connect(viewPhysicsShapeAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

        auto* viewLightScopeAction = new QAction("Light Scope", this);
        viewLightScopeAction->setEnabled(false);
        viewLightScopeAction->setCheckable(true);
        viewLightScopeAction->setChecked(true);
        viewObjectMenu->addAction(viewLightScopeAction);
        viewActions[SceneDisplayer::LIGHT_SCOPE] = viewLightScopeAction;
        connect(viewLightScopeAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

        auto* viewSoundScopeAction = new QAction("Sound Scope", this);
        viewSoundScopeAction->setEnabled(false);
        viewSoundScopeAction->setCheckable(true);
        viewSoundScopeAction->setChecked(true);
        viewObjectMenu->addAction(viewSoundScopeAction);
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

        auto* configMenu = new QMenu("Config");

        lightMaskNameAction = new QAction("Light Mask Names", this);
        lightMaskNameAction->setEnabled(false);
        configMenu->addAction(lightMaskNameAction);
        connect(lightMaskNameAction, SIGNAL(triggered()), this, SLOT(showLightMaskNameDialog()));

        menu->addMenu(fileMenu);
        menu->addMenu(viewMenu);
        menu->addMenu(configMenu);
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
        scenePanelWidget->getObjectPanelWidget()->addObserver(this, ObjectPanelWidget::OBJECT_SUB_TAB_SELECTION_CHANGED);
        scenePanelWidget->getObjectPanelWidget()->getObjectTableView()->addObserver(this, ObjectTableView::OBJECT_SELECTION_CHANGED);
        scenePanelWidget->getObjectPanelWidget()->getPhysicsWidget()->addObserver(this, PhysicsWidget::OBJECT_BODY_SHAPE_WIDGET_CREATED);
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
        bool refreshObjectsHighlight = false;

        if (dynamic_cast<ScenePanelWidget*>(observable)) {
            if (notificationType == ScenePanelWidget::TAB_SELECTED) {
                executeViewPropertiesChangeAction();
            }
        } else if (dynamic_cast<ObjectTableView*>(observable)) {
            if (notificationType == ObjectTableView::OBJECT_SELECTION_CHANGED) {
                refreshObjectsHighlight = true;
            }
        } else if (dynamic_cast<AbstractController*>(observable)) {
            if (notificationType == AbstractController::CHANGES_DONE) {
                refreshWindowTitle();
            }
        } else if (const auto* objectPanelWidget = dynamic_cast<ObjectPanelWidget*>(observable)) {
            if (notificationType == ObjectPanelWidget::OBJECT_SUB_TAB_SELECTION_CHANGED) {
                objectTabSelected = objectPanelWidget->getTabSelected();
                refreshObjectsHighlight = true;
            }
        }
        handleCompoundShapeSelectionChange(observable, notificationType);

        if (refreshObjectsHighlight) {
            const std::vector<const ObjectEntity*>& selectedObjectEntities = scenePanelWidget->getObjectPanelWidget()->getObjectTableView()->getAllSelectedObjectEntities();
            sceneDisplayerWindow->setHighlightObjectPhysicsShapes(selectedObjectEntities);
            sceneDisplayerWindow->setHighlightObjectLights(objectTabSelected == ObjectPanelWidget::ObjectTab::LIGHT ? selectedObjectEntities : std::vector<const ObjectEntity*>{});
            sceneDisplayerWindow->setHighlightObjectSounds(objectTabSelected == ObjectPanelWidget::ObjectTab::SOUND ? selectedObjectEntities : std::vector<const ObjectEntity*>{});
        }
    }

    void MapEditorWindow::handleCompoundShapeSelectionChange(Observable* observable, int notificationType) {
        if (const auto* physicsWidget = dynamic_cast<PhysicsWidget*>(observable)) {
            if (notificationType == PhysicsWidget::OBJECT_BODY_SHAPE_WIDGET_CREATED) {
                BodyShapeWidget* bodyShapeWidget = physicsWidget->getBodyShapeWidget();
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

    void MapEditorWindow::showLightMaskNameDialog() {
        LightMaskNameDialog lightMaskConfigDialog(this, sceneController->getLightMaskNames());
        lightMaskConfigDialog.exec();

        if (lightMaskConfigDialog.result() == QDialog::Accepted) {
            sceneController->setLightMaskNames(lightMaskConfigDialog.getLightMaskNames());
            sceneController->forceModified();
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
        lightMaskNameAction->setEnabled(hasMapOpen);

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

            auto selectedTab = ScenePanelWidget::MainTab::OBJECTS;
            if (scenePanelWidget != nullptr) {
                selectedTab = scenePanelWidget->getTabSelected();
            }

            bool isViewChecked = viewActions[viewProperties]->isChecked();
            bool isCorrespondingTabSelected = getConcernedTabFor(viewProperties) == selectedTab;

            sceneDisplayerWindow->setViewProperties(viewProperties, isViewChecked && isCorrespondingTabSelected);
        }
    }

    ScenePanelWidget::MainTab MapEditorWindow::getConcernedTabFor(SceneDisplayer::ViewProperties viewProperties) {
        if (SceneDisplayer::PHYSICS_SHAPE == viewProperties || SceneDisplayer::LIGHT_SCOPE == viewProperties || SceneDisplayer::SOUND_SCOPE == viewProperties) {
            return ScenePanelWidget::MainTab::OBJECTS;
        }
        if (SceneDisplayer::NAV_MESH == viewProperties) {
            return ScenePanelWidget::MainTab::AI;
        }

        throw std::runtime_error("Impossible to find concerned tab for properties: " + std::to_string(viewProperties));
    }

}
