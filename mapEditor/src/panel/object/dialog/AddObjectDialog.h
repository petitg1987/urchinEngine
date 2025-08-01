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
            AddObjectDialog(QWidget*, const ObjectController*);

            std::unique_ptr<ObjectEntity> moveObjectEntity();

        private:
            void setupNameField(QGridLayout*);
            void setupGroupFields(QGridLayout*);
            std::vector<std::vector<std::string>> getAllGroupHierarchy() const;

            int buildObjectEntity(int);

            void done(int) override;
            bool isObjectEntityExist(std::string_view) const;

            static constexpr std::string GROUP_DELIMITER = " > ";

            const ObjectController* objectController;

            QLabel* objectNameLabel;
            QLineEdit* objectNameText;
            QComboBox* groupComboBox;

            std::unique_ptr<ObjectEntity> objectEntity;
    };

}
