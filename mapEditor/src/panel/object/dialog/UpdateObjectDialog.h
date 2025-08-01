#pragma once

#include <string>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>

#include "controller/object/ObjectController.h"

namespace urchin {

    class UpdateObjectDialog final : public QDialog {
        Q_OBJECT

        public:
            UpdateObjectDialog(QWidget*, std::string, const std::vector<std::string>&, const ObjectController*);

            std::string getNewObjectName() const;
            std::vector<std::string> getNewGroupHierarchy() const;

        private:
            void setupNameFields(QGridLayout*);
            void setupGroupFields(QGridLayout*, const std::vector<std::string>&);

            void done(int) override;
            bool isObjectEntityExist(std::string_view) const;

            std::string originalName;
            const ObjectController* objectController;

            QLabel* updatedObjectNameLabel;
            QLineEdit* updatedObjectNameText;
            QComboBox* updatedGroupComboBox;
            QLineEdit* newGroupText;
    };

}
