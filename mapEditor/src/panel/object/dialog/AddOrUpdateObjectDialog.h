#pragma once

#include <string>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>

#include "controller/object/ObjectController.h"

namespace urchin {

    class AddOrUpdateObjectDialog final : public QDialog {
        Q_OBJECT

        public:
            AddOrUpdateObjectDialog(QWidget*, std::string, const std::vector<std::string>&, const ObjectController&);

            std::unique_ptr<ObjectEntity> getNewObjectEntity();
            std::string getObjectName() const;
            std::vector<std::string> getGroupHierarchy() const;

        private:
            void setupNameFields(QGridLayout*);
            void setupGroupFields(QGridLayout*, const std::vector<std::string>&);

            void done(int) override;
            bool isObjectEntityExist(std::string_view) const;

            std::string defaultName;
            const ObjectController& objectController;

            QLabel* objectNameLabel;
            QLineEdit* objectNameText;
            QComboBox* groupComboBox;
            QLineEdit* newGroupText;
            QLabel* groupResultText;
    };

}
