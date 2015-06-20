#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include "UrchinCommon.h"
#include "NewSoundDialog.h"
#include "support/LabelStyleHelper.h"

namespace urchin
{

	QString NewSoundDialog::preferredSoundPath = QString();

	NewSoundDialog::NewSoundDialog(QWidget *parent, const SoundController *soundController) :
		QDialog(parent),
		soundController(soundController),
		sceneSound(nullptr)
	{
		this->setWindowTitle("New Sound");
		this->resize(530, 160);

		setupNameFields();
		setupSoundFilenameFields();
		setupSoundTypeFields();

		QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
		buttonBox->setGeometry(QRect(30, 120, 475, 22));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

		QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	void NewSoundDialog::setupNameFields()
	{
		soundNameLabel = new QLabel(this);
		soundNameLabel->setText("Sound Name:");
		soundNameLabel->setGeometry(QRect(10, 20, 110, 22));

		soundNameText = new QLineEdit(this);
		soundNameText->setGeometry(QRect(120, 20, 360, 22));
	}

	void NewSoundDialog::setupSoundFilenameFields()
	{
		soundFilenameLabel = new QLabel(this);
		soundFilenameLabel->setText("Sound File:");
		soundFilenameLabel->setGeometry(QRect(10, 50, 110, 22));

		soundFilenameText = new QLineEdit(this);
		soundFilenameText->setReadOnly(true);
		soundFilenameText->setGeometry(QRect(120, 50, 360, 22));

		QPushButton *selectMeshFileButton = new QPushButton(this);
		selectMeshFileButton->setText("...");
		selectMeshFileButton->setGeometry(QRect(485, 50, 22, 22));
		connect(selectMeshFileButton, SIGNAL(clicked()), this, SLOT(showSoundFilenameDialog()));
	}


	void NewSoundDialog::setupSoundTypeFields()
	{
		soundTypeLabel = new QLabel(this);
		soundTypeLabel->setText("Sound Type:");
		soundTypeLabel->setGeometry(QRect(10, 80, 110, 22));

		soundTypeComboBox = new QComboBox(this);
		soundTypeComboBox->setGeometry(QRect(120, 80, 175, 22));
		soundTypeComboBox->addItem(AMBIENT_SOUND_LABEL, QVariant(Sound::SoundType::AMBIENT));
		soundTypeComboBox->addItem(POINT_SOUND_LABEL, QVariant(Sound::SoundType::POINT));
	}

	void NewSoundDialog::updateSoundName()
	{
		QString soundName = soundNameText->text();
		if(!soundName.isEmpty())
		{
			this->soundName = soundName.toUtf8().constData();
		}
	}

	int NewSoundDialog::buildSceneSound(int result)
	{
		try
		{
			sceneSound = new SceneSound();

			sceneSound->setName(soundName);

			std::string workingDirectory = FileSystem::instance()->getWorkingDirectory();
			std::string relativeSoundFilename = FileHandler::getRelativePath(workingDirectory, soundFilename);

			QVariant variant = soundTypeComboBox->currentData();
			Sound::SoundType soundType = static_cast<Sound::SoundType>(variant.toInt());

			Sound *sound;
			if(soundType==Sound::AMBIENT)
			{
				sound = new AmbientSound(relativeSoundFilename);
			}else if(soundType==Sound::POINT)
			{
				sound = new PointSound(relativeSoundFilename, Point3<float>(0.0, 0.0, 0.0));
			}else
			{
				throw std::invalid_argument("Unknown the sound type to create a new sound: " + soundType);
			}

			SoundTrigger *soundTrigger = new ManualTrigger(SoundBehavior(SoundBehavior::PLAY_ONCE, SoundBehavior::INSTANT_STOP));

			sceneSound->setSoundElements(sound, soundTrigger);
		}catch(std::exception &e)
		{
			QMessageBox::critical(this, "Error", e.what());
			if(sceneSound!=nullptr)
			{
				delete sceneSound;
			}

			return QDialog::Rejected;
		}

		return result;
	}


	SceneSound *NewSoundDialog::getSceneSound() const
	{
		return sceneSound;
	}

	void NewSoundDialog::showSoundFilenameDialog()
	{
		QString directory = preferredSoundPath.isEmpty() ? QString::fromStdString(FileSystem::instance()->getWorkingDirectory()) : preferredSoundPath;
		QString filename = QFileDialog::getOpenFileName(this, tr("Open sound file"), directory, "Sound file (*.wav)", 0, QFileDialog::DontUseNativeDialog);
		if(!filename.isNull())
		{
			this->soundFilename = filename.toUtf8().constData();
			this->soundFilenameText->setText(filename);

			std::string preferredSoundPathString = FileHandler::getDirectoryFrom(soundFilename);
			preferredSoundPath = QString::fromStdString(preferredSoundPathString);
		}
	}

	void NewSoundDialog::done(int r)
	{
		if(QDialog::Accepted == r)
		{
			bool hasError = false;

			updateSoundName();
			LabelStyleHelper::applyNormalStyle(soundNameLabel);
			LabelStyleHelper::applyNormalStyle(soundFilenameLabel);

			if(soundName.compare("")==0)
			{
				LabelStyleHelper::applyErrorStyle(soundNameLabel, "Sound name is mandatory");
				hasError = true;
			}else if(isSceneSoundExist(soundName))
			{
				LabelStyleHelper::applyErrorStyle(soundNameLabel, "Sound name is already used");
				hasError = true;
			}
			if(soundFilename.compare("")==0)
			{
				LabelStyleHelper::applyErrorStyle(soundFilenameLabel, "Sound file is mandatory");
				hasError = true;
			}

			if(!hasError)
			{
				r = buildSceneSound(r);
				QDialog::done(r);
			}
		}else
		{
			QDialog::done(r);
		}
	}

	bool NewSoundDialog::isSceneSoundExist(const std::string &name)
	{
		std::list<const SceneSound *> sceneSounds = soundController->getSceneSounds();
		for(std::list<const SceneSound *>::const_iterator it = sceneSounds.begin(); it!=sceneSounds.end(); ++it)
		{
			if((*it)->getName().compare(name)==0)
			{
				return true;
			}
		}

		return false;
	}

}
