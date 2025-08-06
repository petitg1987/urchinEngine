#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <string>

namespace urchin {

    class ChangeMeshesFileDialog final : public QDialog {
        Q_OBJECT

        public:
            ChangeMeshesFileDialog(QWidget*, const std::string&);

            std::string getMeshesFilename() const;

        private:
            void setupMeshesFilenameFields(QGridLayout*, const std::string&);

            void setupFilename(const std::string&) const;

            QLineEdit* meshesFilenameText;

            std::string meshesFilename;
            static QString preferredMeshesPath;

        private slots:
            void showMeshesFilenameDialog();
            void clearMeshesFilename();
    };

}
