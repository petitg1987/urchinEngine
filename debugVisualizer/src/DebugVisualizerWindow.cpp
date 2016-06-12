#include <QApplication>
#include <QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QFileDialog>

#include "DebugVisualizerWindow.h"
#include "UrchinCommon.h"

namespace urchin
{

	DebugVisualizerWindow::DebugVisualizerWindow(const std::string &debugVisualizerPath) :
		debugVisualizerPath(debugVisualizerPath),
		preferredDebugVisualizerPath("./"),
		sceneController(new SceneController())
	{
	    this->setAttribute(Qt::WA_DeleteOnClose);
		this->setWindowTitle(WINDOW_TITLE);
		this->resize(1200, 675);
		QWidget *centralWidget = new QWidget(this);

		QHBoxLayout *horizontalLayout = new QHBoxLayout(centralWidget);
		horizontalLayout->setSpacing(6);
		horizontalLayout->setContentsMargins(0, 0, 0, 0);

		setupMenu();
		setupStatusBar();

		setupSceneDisplayerWidget(centralWidget, horizontalLayout);

		this->setCentralWidget(centralWidget);
	}

	DebugVisualizerWindow::~DebugVisualizerWindow()
	{
		delete sceneController;
	}

	void DebugVisualizerWindow::setupMenu()
	{
		QMenuBar *menu = new QMenuBar(this);

		QMenu* fileMenu = new QMenu("File");

		QAction *openAction = new QAction(this);
		openAction->setText("Open...");
		openAction->setShortcut(QKeySequence("Ctrl+O"));
		fileMenu->addAction(openAction);
		connect(openAction, SIGNAL(triggered()), this, SLOT(showOpenDialog()));

		fileMenu->addSeparator();

		QAction *exitAction = new QAction(this);
		exitAction->setText("Exit");
		fileMenu->addAction(exitAction);
		connect(exitAction, SIGNAL(triggered()), this, SLOT(executeExitAction()));

		QMenu* actionMenu = new QMenu("Action");

		actionNextStepAction = new QAction(this);
		actionNextStepAction->setText("Next step");
		actionNextStepAction->setEnabled(false);
		actionNextStepAction->setShortcut(QKeySequence("Ctrl+Y"));
		actionMenu->addAction(actionNextStepAction);
		connect(actionNextStepAction, SIGNAL(triggered()), this, SLOT(executeNextAction()));

		actionReverseStepAction = new QAction(this);
		actionReverseStepAction->setText("Previous step");
		actionReverseStepAction->setEnabled(false);
		actionReverseStepAction->setShortcut(QKeySequence("Ctrl+Z"));
		actionMenu->addAction(actionReverseStepAction);
		connect(actionReverseStepAction, SIGNAL(triggered()), this, SLOT(reverseLastAction()));

		menu->addMenu(fileMenu);
		menu->addMenu(actionMenu);
		this->setMenuBar(menu);
	}

	void DebugVisualizerWindow::setupStatusBar()
	{
		QStatusBar *statusBar = new QStatusBar(this);
		this->setStatusBar(statusBar);
	}

	void DebugVisualizerWindow::setupSceneDisplayerWidget(QWidget *centralWidget, QHBoxLayout *horizontalLayout)
	{
		sceneDisplayerWidget = new SceneDisplayerWidget(centralWidget, debugVisualizerPath);
		sceneDisplayerWidget->setMouseTracking(true);
		sceneDisplayerWidget->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
		QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(sceneDisplayerWidget->sizePolicy().hasHeightForWidth());
		sceneDisplayerWidget->setSizePolicy(sizePolicy);

		horizontalLayout->addWidget(sceneDisplayerWidget);
	}

	void DebugVisualizerWindow::showOpenDialog()
	{
		QString filename = QFileDialog::getOpenFileName(this, tr("Open file"), preferredDebugVisualizerPath, "Urchin Debug file (*.udbg)", 0, QFileDialog::DontUseNativeDialog);
		if(!filename.isNull() && !filename.isEmpty())
		{
			std::shared_ptr<GeometryEntityHandler> geometryEntityHandler = std::make_shared<GeometryEntityHandler>();
			sceneDisplayerWidget->openDebugFile(geometryEntityHandler);
			sceneController->openDebugFile(filename.toUtf8().constData(), geometryEntityHandler);

			updateDebugVisualizerFilename(filename);
			updateMenuStatus();
		}
	}

	void DebugVisualizerWindow::executeExitAction()
	{
		close();
		QApplication::quit();
	}

	void DebugVisualizerWindow::executeNextAction()
	{
		sceneController->executeNextAction();
	}

	void DebugVisualizerWindow::reverseLastAction()
	{
		sceneController->reverseLastAction();
	}

	void DebugVisualizerWindow::updateMenuStatus()
	{
		bool hasDebugFileOpen = sceneController->hasDebugFileOpen();

		actionNextStepAction->setEnabled(hasDebugFileOpen);
		actionReverseStepAction->setEnabled(hasDebugFileOpen);
	}

	void DebugVisualizerWindow::updateDebugVisualizerFilename(QString qDebugVisualizerFilename)
	{
		debugFilename = qDebugVisualizerFilename.toUtf8().constData();

		if(qDebugVisualizerFilename.isEmpty())
		{
			this->setWindowTitle(WINDOW_TITLE);
		}else
		{
			std::string preferredDebugVisualizerPathString = FileHandler::getDirectoryFrom(debugFilename);
			preferredDebugVisualizerPath = QString::fromStdString(preferredDebugVisualizerPathString);

			this->setWindowTitle(QString::fromStdString(std::string(WINDOW_TITLE) + " (" + debugFilename + ")"));
		}
	}
}
