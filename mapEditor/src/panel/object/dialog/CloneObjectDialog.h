#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include "controller/object/ObjectController.h"

namespace urchin {

    class CloneObjectDialog final : public QDialog {
            Q_OBJECT

        public:
            CloneObjectDialog(QWidget*, const std::string&, std::vector<std::string>, const ObjectController&);

            std::string getObjectName() const;
            std::vector<std::string> getGroupHierarchy() const;

        private:
            void setupNameFields(QGridLayout*);
            void setupGroupFields(QGridLayout*);

            void done(int) override;
            bool isObjectEntityExist() const;

            std::string proposedName;
            std::vector<std::string> originalGroupHierarchy;
            const ObjectController& objectController;

            QLabel* objectNameLabel;
            QLineEdit* objectNameText;
            QComboBox* groupComboBox;
            QLineEdit* newGroupText;
            QLabel* groupResultText;
    };

}
