#include "MapEditorWindow.h"
#include "dialog/NewDialog.h"
#include "dialog/NotSavedDialog.h"
#include "scene/controller/objects/ObjectTableView.h"
#include "scene/controller/objects/bodyshape/BodyCompoundShapeWidget.h"
#include "scene/controller/objects/bodyshape/support/LocalizedShapeTableView.h"
#include "scene/controller/lights/LightTableView.h"
#include "scene/controller/sounds/SoundTableView.h"

#include <stdexcept>
#include <QApplication>
#include <QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QFileDialog>

namespace urchin
{

	MapEditorWindow::MapEditorWindow(const std::string &mapEditorPath) :
		mapEditorPath(mapEditorPath),
		preferredMapPath("./"),
		sceneDisplayerWidget(nullptr),
		sceneControllerWidget(nullptr)
	{
		this->setAttribute(Qt::WA_DeleteOnClose);
		this->setWindowTitle(WINDOW_TITLE);
		this->resize(1200, 675);
		QWidget *centralWidget = new QWidget(this);

		auto *horizontalLayout = new QHBoxLayout(centralWidget);
		horizontalLayout->setSpacing(6);
		horizontalLayout->setContentsMargins(0, 0, 0, 0);

		setupMenu();
		setupStatusBar();

		setupSceneDisplayerWidget(centralWidget, horizontalLayout);
		setupSceneControllerWidget(centralWidget, horizontalLayout);

		this->setCentralWidget(centralWidget);
	}

	void MapEditorWindow::setupMenu()
	{
		auto *menu = new QMenuBar(this);

		QMenu* fileMenu = new QMenu("File");

		QAction *newAction = new QAction("New...", this);
		newAction->setShortcut(QKeySequence("Ctrl+N"));
		fileMenu->addAction(newAction);
		connect(newAction, SIGNAL(triggered()), this, SLOT(showNewDialog()));

		QAction *openAction = new QAction("Open...", this);
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

		fileMenu->addSeparator();

		QAction *exitAction = new QAction("Exit", this);
		fileMenu->addAction(exitAction);
		connect(exitAction, SIGNAL(triggered()), this, SLOT(executeExitAction()));

		QMenu *viewMenu = new QMenu("View");

		QMenu *viewObjectMenu = new QMenu("Object");
		viewMenu->addMenu(viewObjectMenu);
		QAction *viewPhysicsShapeAction = new QAction("Physics Shape", this);
		viewPhysicsShapeAction->setEnabled(false);
		viewPhysicsShapeAction->setCheckable(true);
		viewPhysicsShapeAction->setChecked(true);
		viewObjectMenu->addAction(viewPhysicsShapeAction);
		viewActions[SceneDisplayer::MODEL_PHYSICS] = viewPhysicsShapeAction;
		connect(viewPhysicsShapeAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

		QMenu *viewLightMenu = new QMenu("Light");
		viewMenu->addMenu(viewLightMenu);
		QAction *viewLightScopeAction = new QAction("Light Scope", this);
		viewLightScopeAction->setEnabled(false);
		viewLightScopeAction->setCheckable(true);
		viewLightScopeAction->setChecked(true);
		viewLightMenu->addAction(viewLightScopeAction);
		viewActions[SceneDisplayer::LIGHT_SCOPE] = viewLightScopeAction;
		connect(viewLightScopeAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

		QMenu *viewSoundMenu = new QMenu("Sound");
		viewMenu->addMenu(viewSoundMenu);
		QAction *viewSoundTriggerAction = new QAction("Sound Trigger", this);
		viewSoundTriggerAction->setEnabled(false);
		viewSoundTriggerAction->setCheckable(true);
		viewSoundTriggerAction->setChecked(true);
		viewSoundMenu->addAction(viewSoundTriggerAction);
		viewActions[SceneDisplayer::SOUND_TRIGGER] = viewSoundTriggerAction;
		connect(viewSoundTriggerAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

		QMenu *viewAIMenu = new QMenu("AI");
		viewMenu->addMenu(viewAIMenu);
		QAction *viewNavMeshAction = new QAction("Navigation Mesh", this);
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

	void MapEditorWindow::setupStatusBar()
	{
		auto *statusBar = new QStatusBar(this);
		this->setStatusBar(statusBar);
	}

	void MapEditorWindow::setupSceneDisplayerWidget(QWidget *centralWidget, QHBoxLayout *horizontalLayout)
	{
		sceneDisplayerWidget = new SceneDisplayerWidget(centralWidget, mapEditorPath);
		sceneDisplayerWidget->setMouseTracking(true);
		sceneDisplayerWidget->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
		QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(sceneDisplayerWidget->sizePolicy().hasHeightForWidth());
		sceneDisplayerWidget->setSizePolicy(sizePolicy);

		executeViewPropertiesChangeAction();

		horizontalLayout->addWidget(sceneDisplayerWidget);
	}

	void MapEditorWindow::setupSceneControllerWidget(QWidget *centralWidget, QHBoxLayout *horizontalLayout)
	{
		sceneControllerWidget = new SceneControllerWidget(centralWidget);
		QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(sceneControllerWidget->sizePolicy().hasHeightForWidth());
		sceneControllerWidget->setSizePolicy(sizePolicy);
		sceneControllerWidget->setMaximumSize(QSize(380, 16777215));

		sceneControllerWidget->getObjectControllerWidget()->getObjectTableView()->addObserver(this, ObjectTableView::SELECTION_CHANGED);
		sceneControllerWidget->getObjectControllerWidget()->addObserver(this, ObjectControllerWidget::BODY_SHAPE_INITIALIZED);
		sceneControllerWidget->getLightControllerWidget()->getLightTableView()->addObserver(this, LightTableView::SELECTION_CHANGED);
		sceneControllerWidget->getSoundControllerWidget()->getSoundTableView()->addObserver(this, SoundTableView::SELECTION_CHANGED);
		sceneControllerWidget->addObserver(this, SceneControllerWidget::TAB_SELECTED);

		horizontalLayout->addWidget(sceneControllerWidget);

		sceneDisplayerWidget->addObserver(sceneControllerWidget->getObjectControllerWidget(), SceneDisplayerWidget::BODY_PICKED);
	}

	void MapEditorWindow::notify(Observable *observable, int notificationType)
	{
		if(dynamic_cast<SceneControllerWidget *>(observable)!=nullptr)
		{
			switch(notificationType)
			{
				case SceneControllerWidget::TAB_SELECTED:
					executeViewPropertiesChangeAction();
					break;
				default:
					;
			}
		}else if(auto *objectTableView = dynamic_cast<ObjectTableView *>(observable))
		{
			switch(notificationType)
			{
				case ObjectTableView::SELECTION_CHANGED:
					sceneDisplayerWidget->setHighlightSceneObject(objectTableView->getSelectedSceneObject());
					break;
				default:
					;
			}
		}else if(auto *lightTableView = dynamic_cast<LightTableView *>(observable))
		{
			switch(notificationType)
			{
				case LightTableView::SELECTION_CHANGED:
					sceneDisplayerWidget->setHighlightSceneLight(lightTableView->getSelectedSceneLight());
					break;
				default:
					;
			}
		}else if(auto *soundTableView = dynamic_cast<SoundTableView *>(observable))
		{
			switch(notificationType)
			{
				case SoundTableView::SELECTION_CHANGED:
					sceneDisplayerWidget->setHighlightSceneSound(soundTableView->getSelectedSceneSound());
					break;
				default:
					;
			}
		}

		handleCompoundShapeSelectionChange(observable, notificationType);
	}

	void MapEditorWindow::handleCompoundShapeSelectionChange(Observable *observable, int notificationType)
	{
		if(auto *objectControllerWidget = dynamic_cast<ObjectControllerWidget *>(observable))
		{
			switch(notificationType)
			{
				case ObjectControllerWidget::BODY_SHAPE_INITIALIZED:
				{
					BodyShapeWidget *bodyShapeWidget = objectControllerWidget->getBodyShapeWidget();
					if (auto *bodyCompoundShapeWidget = dynamic_cast<BodyCompoundShapeWidget *>(bodyShapeWidget))
					{
						bodyCompoundShapeWidget->getLocalizedShapeTableView()->addObserver(this, LocalizedShapeTableView::SELECTION_CHANGED);
					}
					break;
				}
				default:
					;
			}
		}else if(auto *localizedShapeTableView = dynamic_cast<LocalizedShapeTableView *>(observable))
		{
			switch(notificationType)
			{
				case LocalizedShapeTableView::SELECTION_CHANGED:
                {
                    sceneDisplayerWidget->setHighlightCompoundShapeComponent(localizedShapeTableView->getSelectedLocalizedShape());
                    break;
                }
				default:
					;
			}
		}
	}

	void MapEditorWindow::showNewDialog()
	{
		if(checkCurrentMapSaved())
		{
			NewDialog newDialog(this);
			newDialog.exec();

			if(newDialog.result()==QDialog::Accepted)
			{
				MapHandler *mapHandler = sceneDisplayerWidget->newMap(newDialog.getFilename(), newDialog.getRelativeWorkingDirectory());
				sceneControllerWidget->newMap(mapHandler, newDialog.getRelativeWorkingDirectory());

				updateMapFilename(QString::fromStdString(newDialog.getFilename()));
				updateMenuStatus();
			}
		}
	}

	void MapEditorWindow::showOpenDialog()
	{
		if(checkCurrentMapSaved())
		{
			QString filename = QFileDialog::getOpenFileName(this, tr("Open file"), preferredMapPath, "XML file (*.xml)", nullptr, QFileDialog::DontUseNativeDialog);
			if(!filename.isNull())
			{
				MapHandler *mapHandler = sceneDisplayerWidget->openMap(filename.toUtf8().constData());
				sceneControllerWidget->openMap(mapHandler);

				updateMapFilename(filename);
				updateMenuStatus();
			}
		}
	}

	void MapEditorWindow::executeSaveAction()
	{
		sceneDisplayerWidget->saveMap(mapFilename);
		sceneControllerWidget->saveMap(mapFilename);
	}

	void MapEditorWindow::showSaveAsDialog()
	{
		QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), preferredMapPath, "XML file (*.xml)", nullptr, QFileDialog::DontUseNativeDialog);
		if(!filename.isNull())
		{
			std::string filenameString = filename.toUtf8().constData();
			std::string fileExtension = FileHandler::getFileExtension(filenameString);
			if(!StringUtil::insensitiveEquals(fileExtension, ".xml"))
			{
				filenameString += ".xml";
			}

			sceneDisplayerWidget->saveMap(filenameString);
			sceneControllerWidget->saveMap(filenameString);

			updateMapFilename(QString::fromStdString(filenameString));
		}
	}

	bool MapEditorWindow::executeCloseAction()
	{
		bool canProceed = false;
		if(checkCurrentMapSaved())
		{
			sceneDisplayerWidget->closeMap();
			sceneControllerWidget->closeMap();

			updateMapFilename("");
			updateMenuStatus();

			canProceed = true;
		}

		return canProceed;
	}

	void MapEditorWindow::executeExitAction()
	{
		if(executeCloseAction())
		{
			QApplication::quit();
		}
	}

	void MapEditorWindow::closeEvent(QCloseEvent *event)
	{
		if(executeCloseAction())
		{
			close();
			QApplication::quit();
		}else
		{
			event->ignore();
		}
	}

	bool MapEditorWindow::checkCurrentMapSaved()
	{
		bool canProceed = true;
		if(sceneControllerWidget->hasMapOpen() && sceneControllerWidget->isModified())
		{
			NotSavedDialog notSavedDialog(this);
			notSavedDialog.exec();

			if(notSavedDialog.result()==QDialog::Accepted && notSavedDialog.needSave())
			{
				executeSaveAction();
			}else if(notSavedDialog.result()==QDialog::Rejected)
			{
				canProceed = false;
			}
		}

		return canProceed;
	}

	void MapEditorWindow::updateMenuStatus()
	{
		bool hasMapOpen = sceneControllerWidget->hasMapOpen();

		saveAction->setEnabled(hasMapOpen);
		saveAsAction->setEnabled(hasMapOpen);
		closeAction->setEnabled(hasMapOpen);
		for(auto &viewAction : viewActions)
		{
			viewAction.second->setEnabled(hasMapOpen);
		}
	}

	void MapEditorWindow::updateMapFilename(QString qMapFilename)
	{
		mapFilename = qMapFilename.toUtf8().constData();

		if(qMapFilename.isEmpty())
		{
			this->setWindowTitle(WINDOW_TITLE);
		}else
		{
			std::string preferredMapPathString = FileHandler::getDirectoryFrom(mapFilename);
			preferredMapPath = QString::fromStdString(preferredMapPathString);

			this->setWindowTitle(QString::fromStdString(std::string(WINDOW_TITLE) + " (" + mapFilename + ")"));
		}
	}

	void MapEditorWindow::executeViewPropertiesChangeAction()
	{
		for(int i=0; i<SceneDisplayer::LAST_VIEW_PROPERTIES; ++i)
		{
			auto viewProperties = static_cast<SceneDisplayer::ViewProperties>(i);

			bool isViewChecked = viewActions[viewProperties]->isChecked();
			bool isCorrespondingTabSelected = (sceneControllerWidget==nullptr && i==0)
					|| (sceneControllerWidget!=nullptr && getConcernedTabFor(viewProperties)==sceneControllerWidget->getTabSelected());

			sceneDisplayerWidget->setViewProperties(viewProperties, isViewChecked && isCorrespondingTabSelected);
		}
	}

	SceneControllerWidget::TabName MapEditorWindow::getConcernedTabFor(SceneDisplayer::ViewProperties viewProperties)
	{
		if(SceneDisplayer::MODEL_PHYSICS==viewProperties)
		{
			return SceneControllerWidget::OBJECTS;
		}
		if(SceneDisplayer::LIGHT_SCOPE==viewProperties)
		{
			return SceneControllerWidget::LIGHTS;
		}
		if(SceneDisplayer::SOUND_TRIGGER==viewProperties)
		{
			return SceneControllerWidget::SOUNDS;
		}
		if(SceneDisplayer::NAV_MESH==viewProperties)
		{
			return SceneControllerWidget::AI;
		}

		throw std::runtime_error("Impossible to find concerned tab for properties: " + viewProperties);
	}

}
