#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include "UrchinCommon.h"

#include "NewObjectDialog.h"
#include "support/style/LabelStyleHelper.h"
#include "support/style/ButtonStyleHelper.h"

namespace urchin
{

	QString NewObjectDialog::preferredMeshPath = QString();

	NewObjectDialog::NewObjectDialog(QWidget *parent, const ObjectController *objectController) :
		QDialog(parent),
		objectController(objectController),
        objectNameLabel(nullptr),
        objectNameText(nullptr),
        meshFilenameLabel(nullptr),
        meshFilenameText(nullptr),
		sceneObject(nullptr)
	{
		this->setWindowTitle("New Object");
		this->resize(530, 130);
		this->setFixedSize(this->width(),this->height());

		auto *mainLayout = new QGridLayout(this);
		mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

		setupNameFields(mainLayout);
		setupMeshFilenameFields(mainLayout);

		auto *buttonBox = new QDialogButtonBox();
		mainLayout->addWidget(buttonBox, 2, 0, 1, 3);
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

		QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	void NewObjectDialog::setupNameFields(QGridLayout *mainLayout)
	{
		objectNameLabel = new QLabel("Object Name:");
		mainLayout->addWidget(objectNameLabel, 0, 0);

		objectNameText = new QLineEdit();
		mainLayout->addWidget(objectNameText, 0, 1);
		objectNameText->setFixedWidth(360);
	}

	void NewObjectDialog::setupMeshFilenameFields(QGridLayout *mainLayout)
	{
		meshFilenameLabel = new QLabel("Mesh File:");
		mainLayout->addWidget(meshFilenameLabel, 1, 0);

		meshFilenameText = new QLineEdit();
		mainLayout->addWidget(meshFilenameText, 1, 1);
		meshFilenameText->setReadOnly(true);
		meshFilenameText->setFixedWidth(360);

		auto *selectMeshFileButton = new QPushButton("...");
		mainLayout->addWidget(selectMeshFileButton, 1, 2);
		ButtonStyleHelper::applyNormalStyle(selectMeshFileButton);
		selectMeshFileButton->setFixedWidth(22);
		connect(selectMeshFileButton, SIGNAL(clicked()), this, SLOT(showMeshFilenameDialog()));
	}

	void NewObjectDialog::updateObjectName()
	{
		QString objectName = objectNameText->text();
		if(!objectName.isEmpty())
		{
			this->objectName = objectName.toUtf8().constData();
		}
	}

	int NewObjectDialog::buildSceneObject(int result)
	{
		try
		{
			sceneObject = new SceneObject();

			sceneObject->setName(objectName);

			std::string resourcesDirectory = FileSystem::instance()->getResourcesDirectory();
			std::string relativeMeshFilename;
			if(!meshFilename.empty())
			{
				relativeMeshFilename = FileHandler::getRelativePath(resourcesDirectory, meshFilename);
			}
			auto *model = new Model(relativeMeshFilename);
			sceneObject->setModel(model);
		}catch(std::exception &e)
		{
			QMessageBox::critical(this, "Error", e.what());
			delete sceneObject;

			return QDialog::Rejected;
		}

		return result;
	}


	SceneObject *NewObjectDialog::getSceneObject() const
	{
		return sceneObject;
	}

	void NewObjectDialog::showMeshFilenameDialog()
	{
		QString directory = preferredMeshPath.isEmpty() ? QString::fromStdString(FileSystem::instance()->getResourcesDirectory()) : preferredMeshPath;
		QString filename = QFileDialog::getOpenFileName(this, tr("Open mesh file"), directory, "Mesh file (*.urchinMesh)", nullptr, QFileDialog::DontUseNativeDialog);
		if(!filename.isNull())
		{
			this->meshFilename = filename.toUtf8().constData();
			this->meshFilenameText->setText(filename);

			std::string preferredMeshPathString = FileHandler::getDirectoryFrom(meshFilename);
			preferredMeshPath = QString::fromStdString(preferredMeshPathString);
		}
	}

	void NewObjectDialog::done(int r)
	{
		if(QDialog::Accepted == r)
		{
			bool hasError = false;

			updateObjectName();
			LabelStyleHelper::applyNormalStyle(objectNameLabel);
			LabelStyleHelper::applyNormalStyle(meshFilenameLabel);

			if(objectName.empty())
			{
				LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is mandatory");
				hasError = true;
			}else if(isSceneObjectExist(objectName))
			{
				LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is already used");
				hasError = true;
			}

			if(!hasError)
			{
				r = buildSceneObject(r);
				QDialog::done(r);
			}
		}else
		{
			QDialog::done(r);
		}
	}

	bool NewObjectDialog::isSceneObjectExist(const std::string &name)
	{
		std::list<const SceneObject *> sceneObjects = objectController->getSceneObjects();
		for(auto &sceneObject : sceneObjects)
		{
			if(sceneObject->getName() == name)
			{
				return true;
			}
		}

		return false;
	}

}
