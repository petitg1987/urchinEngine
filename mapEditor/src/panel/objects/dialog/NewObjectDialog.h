#ifndef URCHINENGINE_MAPEDITOR_NEWOBJECTDIALOG_H
#define URCHINENGINE_MAPEDITOR_NEWOBJECTDIALOG_H

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include "UrchinMapHandler.h"
#include "panel/objects/ObjectController.h"

namespace urchin
{

	class NewObjectDialog : public QDialog
	{
		Q_OBJECT

		public:
			NewObjectDialog(QWidget *parent, const ObjectController *);

			SceneObject *getSceneObject() const;

		private:
			void setupNameFields(QGridLayout *);
			void setupMeshFilenameFields(QGridLayout *);

			void updateObjectName();
			int buildSceneObject(int);

			void done(int) override;
			bool isSceneObjectExist(const std::string &);

			const ObjectController *objectController;

			QLabel *objectNameLabel;
			QLineEdit *objectNameText;
			QLabel *meshFilenameLabel;
			QLineEdit *meshFilenameText;

			std::string objectName;
			std::string meshFilename;
			SceneObject *sceneObject;
			static QString preferredMeshPath;

		private slots:
			void showMeshFilenameDialog();
	};

}

#endif
