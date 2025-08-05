#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <string>

namespace urchin {

    class ChangeMeshesFileDialog final : public QDialog {
        Q_OBJECT

        public:
            explicit ChangeMeshesFileDialog(QWidget*);

            std::string getMeshesFilename() const;

        private:
            void setupMeshFilenameFields(QGridLayout*);

            QLineEdit* meshesFilenameText;

            std::string meshesFilename;
            static QString preferredMeshesPath;

        private slots:
            void showMeshFilenameDialog();
    };

}
