#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

namespace urchin {

    class NewDialog final : public QDialog {
        Q_OBJECT

        public:
            explicit NewDialog(QWidget* = nullptr);

            std::string getFilename() const;
            std::string getRelativeWorkingDirectory() const;

        private:
            void setupNameFields(QGridLayout*);
            void setupDirectoryFields(QGridLayout*);
            void setupWorkingDirectoryFields(QGridLayout*);

            void updateMapFilename();
            void updateRelativeWorkingDirectory();

            void done(int) override;

            QLabel* mapNameLabel;
            QLineEdit* mapNameText;
            QLabel* mapDirectoryLabel;
            QLineEdit* mapDirectoryText;
            QLabel* mapWorkingDirectoryLabel;
            QLineEdit* mapWorkingDirectoryText;
            QLabel* mapRelWorkingDirectoryText;

            std::string mapFilename;
            std::string mapDirectory;
            std::string mapWorkingDirectory;
            std::string mapRelWorkingDirectory;

        private slots:
            void showMapDirectoryDialog();
            void showMapWorkingDirectoryDialog();
    };

}
