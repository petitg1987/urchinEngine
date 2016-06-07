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
		preferredDebugVisualizerPath("./")
	{
		this->setWindowTitle(WINDOW_TITLE);
		this->resize(1200, 675);
		QWidget *centralWidget = new QWidget(this);

		QHBoxLayout *horizontalLayout = new QHBoxLayout(centralWidget);
		horizontalLayout->setSpacing(6);
		horizontalLayout->setContentsMargins(11, 11, 11, 11);
		horizontalLayout->setContentsMargins(0, 0, 0, 0);

		setupMenu();
		setupStatusBar();

		setupSceneDisplayerWidget(centralWidget, horizontalLayout);

		this->setCentralWidget(centralWidget);
	}

	DebugVisualizerWindow::~DebugVisualizerWindow()
	{

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
		actionNextStepAction->setShortcut(QKeySequence("Ctrl+Y"));
		actionMenu->addAction(actionNextStepAction);
		connect(actionNextStepAction, SIGNAL(triggered()), this, SLOT(executeNextStepAction()));

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
			debugFilename = filename.toUtf8().constData();
			std::string preferredDebugVisualizerPathString = FileHandler::getDirectoryFrom(debugFilename);
			preferredDebugVisualizerPath = QString::fromStdString(preferredDebugVisualizerPathString);
		}
	}

	void DebugVisualizerWindow::executeExitAction()
	{
		QApplication::quit();
	}

	void DebugVisualizerWindow::executeNextStepAction()
	{
		//TODO
	}
}
