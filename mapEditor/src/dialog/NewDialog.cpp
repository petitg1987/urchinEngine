#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include "UrchinCommon.h"
#include "NewDialog.h"
#include "support/LabelStyleHelper.h"

namespace urchin
{

	NewDialog::NewDialog(QWidget *parent) :
		QDialog(parent)
	{
		this->setWindowTitle("New Map File");
		this->resize(530, 195);

		setupNameFields();
		setupDirectoryFields();
		setupWorkingDirectoryFields();

		QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
		buttonBox->setGeometry(QRect(30, 155, 475, 22));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

		QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	void NewDialog::setupNameFields()
	{
		mapNameLabel = new QLabel("Name (*.xml):", this);
		mapNameLabel->setGeometry(QRect(10, 20, 110, 22));

		mapNameText = new QLineEdit(this);
		mapNameText->setGeometry(QRect(120, 20, 360, 22));
	}

	void NewDialog::setupDirectoryFields()
	{
		mapDirectoryLabel = new QLabel("Directory:", this);
		mapDirectoryLabel->setGeometry(QRect(10, 50, 110, 22));

		mapDirectoryText = new QLineEdit(this);
		mapDirectoryText->setReadOnly(true);
		mapDirectoryText->setGeometry(QRect(120, 50, 360, 22));

		QPushButton *selectMapDirButton = new QPushButton("...", this);
		selectMapDirButton->setGeometry(QRect(485, 50, 22, 22));
		connect(selectMapDirButton, SIGNAL(clicked()), this, SLOT(showMapDirectoryDialog()));
	}

	void NewDialog::setupWorkingDirectoryFields()
	{
		mapWorkingDirectoryLabel = new QLabel("Working Directory:", this);
		mapWorkingDirectoryLabel->setGeometry(QRect(10, 80, 140, 22));

		mapWorkingDirectoryText = new QLineEdit(this);
		mapWorkingDirectoryText->setReadOnly(true);
		mapWorkingDirectoryText->setGeometry(QRect(120, 80, 360, 22));

		QPushButton *selectMapWorkingDirButton = new QPushButton("...", this);
		selectMapWorkingDirButton->setGeometry(QRect(485, 80, 22, 22));
		connect(selectMapWorkingDirButton, SIGNAL(clicked()), this, SLOT(showMapWorkingDirectoryDialog()));

		//relative working directory
		QLabel *mapRelWorkingDirectoryLabel = new QLabel("Relative Working Dir.:", this);
		mapRelWorkingDirectoryLabel->setGeometry(QRect(10, 115, 140, 22));

		mapRelWorkingDirectoryText = new QLabel(this);
		mapRelWorkingDirectoryText->setGeometry(QRect(120, 115, 360, 22));
	}

	std::string NewDialog::getFilename() const
	{
		return mapDirectory + mapFilename;
	}

	std::string NewDialog::getRelativeWorkingDirectory() const
	{
		return mapRelWorkingDirectory;
	}

	void NewDialog::updateMapFilename()
	{
		QString mapFilename = mapNameText->text();
		if(!mapFilename.isEmpty())
		{
			this->mapFilename = mapFilename.toUtf8().constData();
			std::string fileExtension = FileHandler::getFileExtension(this->mapFilename);
			if(!StringUtil::insensitiveEquals(fileExtension, ".xml"))
			{
				this->mapFilename += ".xml";
			}
		}
	}

	void NewDialog::showMapDirectoryDialog()
	{
		QString mapDirectory = QFileDialog::getExistingDirectory(this, tr("Select Directory"), "./", QFileDialog::DontUseNativeDialog|QFileDialog::ShowDirsOnly);
		if(!mapDirectory.isEmpty())
		{
			this->mapDirectory = std::string(mapDirectory.toUtf8().constData()) + "/";

			mapDirectoryText->setText(mapDirectory);
			updateRelativeWorkingDirectory();
		}
	}

	void NewDialog::showMapWorkingDirectoryDialog()
	{
		QString mapWorkingDirectory = QFileDialog::getExistingDirectory(this, tr("Select working Directory"), "./", QFileDialog::DontUseNativeDialog|QFileDialog::ShowDirsOnly);
		if(!mapWorkingDirectory.isEmpty())
		{
			this->mapWorkingDirectory = std::string(mapWorkingDirectory.toUtf8().constData()) + "/";

			mapWorkingDirectoryText->setText(mapWorkingDirectory);
			updateRelativeWorkingDirectory();
		}
	}

	void NewDialog::updateRelativeWorkingDirectory()
	{
		if(mapDirectory.compare("")!=0 && mapWorkingDirectory.compare("")!=0)
		{
			mapRelWorkingDirectory = FileHandler::getRelativePath(mapDirectory, mapWorkingDirectory);
			mapRelWorkingDirectoryText->setText(QString::fromStdString(mapRelWorkingDirectory));
		}
	}

	void NewDialog::done(int r)
	{
		if(QDialog::Accepted == r)
		{
			bool hasError = false;

			updateMapFilename();
			LabelStyleHelper::applyNormalStyle(mapNameLabel);
			LabelStyleHelper::applyNormalStyle(mapDirectoryLabel);
			LabelStyleHelper::applyNormalStyle(mapWorkingDirectoryLabel);

			if(mapFilename.compare("")==0)
			{
				LabelStyleHelper::applyErrorStyle(mapNameLabel, "Map name is mandatory");
				hasError = true;
			}else if(mapFilename.find_last_of("/\\")!=std::string::npos)
			{
				LabelStyleHelper::applyErrorStyle(mapNameLabel, "Map name cannot contains slash character");
				hasError = true;
			}
			if(mapDirectory.compare("")==0)
			{
				LabelStyleHelper::applyErrorStyle(mapDirectoryLabel, "Map directory is mandatory");
				hasError = true;
			}
			if(mapWorkingDirectory.compare("")==0)
			{
				LabelStyleHelper::applyErrorStyle(mapWorkingDirectoryLabel, "Map working directory is mandatory");
				hasError = true;
			}

			if(!hasError)
			{
				QDialog::done(r);
			}
		}else
		{
			QDialog::done(r);
		}
	}

}
