#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include "UrchinCommon.h"
#include "NewObjectDialog.h"
#include "support/LabelStyleHelper.h"

namespace urchin
{

	QString NewObjectDialog::preferredMeshPath = QString();

	NewObjectDialog::NewObjectDialog(QWidget *parent, const ObjectController *objectController) :
		QDialog(parent),
		objectController(objectController),
		sceneObject(nullptr)
	{
		this->setWindowTitle("New Object");
		this->resize(530, 130);

		setupNameFields();
		setupMeshFilenameFields();

		QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
		buttonBox->setGeometry(QRect(30, 90, 475, 22));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

		QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	void NewObjectDialog::setupNameFields()
	{
		objectNameLabel = new QLabel(this);
		objectNameLabel->setText("Object Name:");
		objectNameLabel->setGeometry(QRect(10, 20, 110, 22));

		objectNameText = new QLineEdit(this);
		objectNameText->setGeometry(QRect(120, 20, 360, 22));
	}

	void NewObjectDialog::setupMeshFilenameFields()
	{
		meshFilenameLabel = new QLabel(this);
		meshFilenameLabel->setText("Mesh File:");
		meshFilenameLabel->setGeometry(QRect(10, 50, 110, 22));

		meshFilenameText = new QLineEdit(this);
		meshFilenameText->setReadOnly(true);
		meshFilenameText->setGeometry(QRect(120, 50, 360, 22));

		QPushButton *selectMeshFileButton = new QPushButton(this);
		selectMeshFileButton->setText("...");
		selectMeshFileButton->setGeometry(QRect(485, 50, 22, 22));
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

			std::string workingDirectory = FileSystem::instance()->getWorkingDirectory();
			std::string relativeMeshFilename = FileHandler::getRelativePath(workingDirectory, meshFilename);
			Model *model = new Model(relativeMeshFilename);
			sceneObject->setModel(model);
		}catch(std::exception &e)
		{
			QMessageBox::critical(this, "Error", e.what());
			if(sceneObject!=nullptr)
			{
				delete sceneObject;
			}

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
		QString directory = preferredMeshPath.isEmpty() ? QString::fromStdString(FileSystem::instance()->getWorkingDirectory()) : preferredMeshPath;
		QString filename = QFileDialog::getOpenFileName(this, tr("Open mesh file"), directory, "Mesh file (*.md5mesh)", 0, QFileDialog::DontUseNativeDialog);
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

			if(objectName.compare("")==0)
			{
				LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is mandatory");
				hasError = true;
			}else if(isSceneObjectExist(objectName))
			{
				LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is already used");
				hasError = true;
			}
			if(meshFilename.compare("")==0)
			{
				LabelStyleHelper::applyErrorStyle(meshFilenameLabel, "Mesh is mandatory");
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
		for(std::list<const SceneObject *>::const_iterator it = sceneObjects.begin(); it!=sceneObjects.end(); ++it)
		{
			if((*it)->getName().compare(name)==0)
			{
				return true;
			}
		}

		return false;
	}

}
