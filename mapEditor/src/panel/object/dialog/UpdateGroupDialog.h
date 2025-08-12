#pragma once

#include <string>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>

#include "controller/object/ObjectController.h"

namespace urchin {

    class UpdateGroupDialog final : public QDialog {
        Q_OBJECT

        public:
            UpdateGroupDialog(QWidget*, const std::vector<std::string>&, const ObjectController&);

            std::vector<std::string> getUpdatedGroupHierarchy() const;

        private:
            void setupGroupFields(QGridLayout*);

            void done(int) override;
            bool isObjectEntitiesExist() const;

            std::vector<std::string> originalGroupHierarchy;
            const ObjectController& objectController;

            QComboBox* baseGroupComboBox;
            QLineEdit* groupNameText;
            QLabel* groupResultText;
    };

}
