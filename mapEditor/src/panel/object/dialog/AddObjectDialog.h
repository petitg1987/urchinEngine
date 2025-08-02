#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <UrchinAggregation.h>

#include "controller/object/ObjectController.h"

namespace urchin {

    class AddObjectDialog final : public QDialog {
        Q_OBJECT

        public:
            AddObjectDialog(QWidget*, const std::vector<std::string>&, const ObjectController*);

            std::unique_ptr<ObjectEntity> moveObjectEntity();

        private:
            void setupNameField(QGridLayout*);
            void setupGroupFields(QGridLayout*, const std::vector<std::string>&);

            std::vector<std::string> getGroupHierarchy() const;
            int buildObjectEntity(int);

            void done(int) override;
            bool isObjectEntityExist(std::string_view) const;

            const ObjectController* objectController;

            QLabel* objectNameLabel;
            QLineEdit* objectNameText;
            QComboBox* groupComboBox;
            QLineEdit* newGroupText;
            QLabel* groupResultText;

            std::unique_ptr<ObjectEntity> objectEntity;
    };

}
