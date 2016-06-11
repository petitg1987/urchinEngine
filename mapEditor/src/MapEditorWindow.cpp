#include <QApplication>
#include <QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QFileDialog>

#include "MapEditorWindow.h"
#include "dialog/NewDialog.h"
#include "dialog/NotSavedDialog.h"
#include "scene/controller/objects/ObjectTableView.h"
#include "scene/controller/objects/bodyshape/BodyCompoundShapeWidget.h"
#include "scene/controller/objects/bodyshape/support/LocalizedShapeTableView.h"
#include "scene/controller/lights/LightTableView.h"
#include "scene/controller/sounds/SoundTableView.h"

namespace urchin
{

	MapEditorWindow::MapEditorWindow(const std::string &mapEditorPath) :
		mapEditorPath(mapEditorPath),
		preferredMapPath("./")
	{
		this->setWindowTitle(WINDOW_TITLE);
		this->resize(1200, 675);
		QWidget *centralWidget = new QWidget(this);

		QHBoxLayout *horizontalLayout = new QHBoxLayout(centralWidget);
		horizontalLayout->setSpacing(6);
		horizontalLayout->setContentsMargins(0, 0, 0, 0);

		setupMenu();
		setupStatusBar();

		setupSceneDisplayerWidget(centralWidget, horizontalLayout);
		setupSceneControllerWidget(centralWidget, horizontalLayout);

		this->setCentralWidget(centralWidget);
	}

	MapEditorWindow::~MapEditorWindow()
	{

	}

	void MapEditorWindow::setupMenu()
	{
		QMenuBar *menu = new QMenuBar(this);

		QMenu* fileMenu = new QMenu("File");

		QAction *newAction = new QAction(this);
		newAction->setText("New...");
		newAction->setShortcut(QKeySequence("Ctrl+N"));
		fileMenu->addAction(newAction);
		connect(newAction, SIGNAL(triggered()), this, SLOT(showNewDialog()));

		QAction *openAction = new QAction(this);
		openAction->setText("Open...");
		openAction->setShortcut(QKeySequence("Ctrl+O"));
		fileMenu->addAction(openAction);
		connect(openAction, SIGNAL(triggered()), this, SLOT(showOpenDialog()));

		saveAction = new QAction(this);
		saveAction->setText("Save");
		saveAction->setEnabled(false);
		saveAction->setShortcut(QKeySequence("Ctrl+S"));
		fileMenu->addAction(saveAction);
		connect(saveAction, SIGNAL(triggered()), this, SLOT(executeSaveAction()));

		saveAsAction = new QAction(this);
		saveAsAction->setText("Save as...");
		saveAsAction->setEnabled(false);
		saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
		fileMenu->addAction(saveAsAction);
		connect(saveAsAction, SIGNAL(triggered()), this, SLOT(showSaveAsDialog()));

		closeAction = new QAction(this);
		closeAction->setText("Close");
		closeAction->setEnabled(false);
		closeAction->setShortcut(QKeySequence("Ctrl+W"));
		fileMenu->addAction(closeAction);
		connect(closeAction, SIGNAL(triggered()), this, SLOT(executeCloseAction()));

		fileMenu->addSeparator();

		QAction *exitAction = new QAction(this);
		exitAction->setText("Exit");
		fileMenu->addAction(exitAction);
		connect(exitAction, SIGNAL(triggered()), this, SLOT(executeExitAction()));

		QMenu* viewMenu = new QMenu("View");

		viewPhysicsShapeAction = new QAction(this);
		viewPhysicsShapeAction->setText("Physics Shape");
		viewPhysicsShapeAction->setEnabled(false);
		viewPhysicsShapeAction->setCheckable(true);
		viewPhysicsShapeAction->setChecked(true);
		viewMenu->addAction(viewPhysicsShapeAction);
		connect(viewPhysicsShapeAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

		viewLightScopeAction = new QAction(this);
		viewLightScopeAction->setText("Light Scope");
		viewLightScopeAction->setEnabled(false);
		viewLightScopeAction->setCheckable(true);
		viewLightScopeAction->setChecked(true);
		viewMenu->addAction(viewLightScopeAction);
		connect(viewLightScopeAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

		viewSoundTriggerAction = new QAction(this);
		viewSoundTriggerAction->setText("Sound Trigger");
		viewSoundTriggerAction->setEnabled(false);
		viewSoundTriggerAction->setCheckable(true);
		viewSoundTriggerAction->setChecked(true);
		viewMenu->addAction(viewSoundTriggerAction);
		connect(viewSoundTriggerAction, SIGNAL(triggered()), this, SLOT(executeViewPropertiesChangeAction()));

		menu->addMenu(fileMenu);
		menu->addMenu(viewMenu);
		this->setMenuBar(menu);
	}

	void MapEditorWindow::setupStatusBar()
	{
		QStatusBar *statusBar = new QStatusBar(this);
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
		sceneControllerWidget->setMaximumSize(QSize(360, 16777215));

		sceneControllerWidget->getObjectControllerWidget()->getObjectTableView()->addObserver(this, ObjectTableView::SELECTION_CHANGED);
		sceneControllerWidget->getObjectControllerWidget()->addObserver(this, ObjectControllerWidget::BODY_SHAPE_INITIALIZED);
		sceneControllerWidget->getLightControllerWidget()->getLightTableView()->addObserver(this, LightTableView::SELECTION_CHANGED);
		sceneControllerWidget->getSoundControllerWidget()->getSoundTableView()->addObserver(this, SoundTableView::SELECTION_CHANGED);

		horizontalLayout->addWidget(sceneControllerWidget);
	}

	void MapEditorWindow::notify(Observable *observable, int notificationType)
	{
		if(ObjectTableView *objectTableView = dynamic_cast<ObjectTableView *>(observable))
		{
			switch(notificationType)
			{
				case ObjectTableView::SELECTION_CHANGED:
					sceneDisplayerWidget->setHighlightSceneObject(objectTableView->getSelectedSceneObject());
					break;
			}
		}else if(LightTableView *lightTableView = dynamic_cast<LightTableView *>(observable))
		{
			switch(notificationType)
			{
				case LightTableView::SELECTION_CHANGED:
					sceneDisplayerWidget->setHighlightSceneLight(lightTableView->getSelectedSceneLight());
					break;
			}
		}else if(SoundTableView *soundTableView = dynamic_cast<SoundTableView *>(observable))
		{
			switch(notificationType)
			{
				case SoundTableView::SELECTION_CHANGED:
					sceneDisplayerWidget->setHighlightSceneSound(soundTableView->getSelectedSceneSound());
					break;
			}
		}

		handleCompoundShapeSelectionChange(observable, notificationType);
	}

	void MapEditorWindow::handleCompoundShapeSelectionChange(Observable *observable, int notificationType)
	{
		if(ObjectControllerWidget *objectControllerWidget = dynamic_cast<ObjectControllerWidget *>(observable))
		{
			switch(notificationType)
			{
				case ObjectControllerWidget::BODY_SHAPE_INITIALIZED:
					BodyShapeWidget *bodyShapeWidget = objectControllerWidget->getBodyShapeWidget();
					if(BodyCompoundShapeWidget *bodyCompoundShapeWidget = dynamic_cast<BodyCompoundShapeWidget *>(bodyShapeWidget))
					{
						bodyCompoundShapeWidget->getLocalizedShapeTableView()->addObserver(this, LocalizedShapeTableView::SELECTION_CHANGED);
					}
					break;
			}
		}else if(LocalizedShapeTableView *localizedShapeTableView = dynamic_cast<LocalizedShapeTableView *>(observable))
		{
			switch(notificationType)
			{
				case LocalizedShapeTableView::SELECTION_CHANGED:
					sceneDisplayerWidget->setHighlightCompoundShapeComponent(localizedShapeTableView->getSelectedLocalizedShape());
					break;
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
			QString filename = QFileDialog::getOpenFileName(this, tr("Open file"), preferredMapPath, "XML file (*.xml)", 0, QFileDialog::DontUseNativeDialog);
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
		QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), preferredMapPath, "XML file (*.xml)", 0, QFileDialog::DontUseNativeDialog);
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
		viewPhysicsShapeAction->setEnabled(hasMapOpen);
		viewLightScopeAction->setEnabled(hasMapOpen);
		viewSoundTriggerAction->setEnabled(hasMapOpen);
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
		sceneDisplayerWidget->setViewProperties(SceneDisplayer::MODEL_PHYSICS, viewPhysicsShapeAction->isChecked());
		sceneDisplayerWidget->setViewProperties(SceneDisplayer::LIGHT_SCOPE, viewLightScopeAction->isChecked());
		sceneDisplayerWidget->setViewProperties(SceneDisplayer::SOUND_TRIGGER, viewSoundTriggerAction->isChecked());
	}

}
