#ifndef ENGINE_MAPEDITOR_NEWDIALOG_H
#define ENGINE_MAPEDITOR_NEWDIALOG_H

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

namespace urchin
{

	class NewDialog : public QDialog
	{
		Q_OBJECT

		public:
			NewDialog(QWidget *parent=0);

			std::string getFilename() const;
			std::string getRelativeWorkingDirectory() const;

		private:
			void setupNameFields(QGridLayout *);
			void setupDirectoryFields(QGridLayout *);
			void setupWorkingDirectoryFields(QGridLayout *);

			void updateMapFilename();
			void updateRelativeWorkingDirectory();

			void done(int);

			QLabel *mapNameLabel;
			QLineEdit *mapNameText;
			QLabel *mapDirectoryLabel;
			QLineEdit *mapDirectoryText;
			QLabel *mapWorkingDirectoryLabel;
			QLineEdit *mapWorkingDirectoryText;
			QLabel *mapRelWorkingDirectoryText;

			std::string mapFilename;
			std::string mapDirectory;
			std::string mapWorkingDirectory;
			std::string mapRelWorkingDirectory;

		private slots:
			void showMapDirectoryDialog();
			void showMapWorkingDirectoryDialog();
	};

}

#endif
