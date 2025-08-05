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
            AddOrUpdateObjectDialog(QWidget*, const ObjectController&, std::string);

            void setDefaultValues(const std::string&, const std::vector<std::string>&, uint8_t);

            std::unique_ptr<ObjectEntity> getNewObjectEntity() const;
            std::string getObjectName() const;
            std::vector<std::string> getGroupHierarchy() const;

        private:
            void setupNameFields(QGridLayout*);
            void setupGroupFields(QGridLayout*);

            void done(int) override;
            bool isObjectEntityExist(std::string_view) const;

            std::string originalName;
            const ObjectController& objectController;
            uint8_t defaultLightMask;

            QLabel* objectNameLabel;
            QLineEdit* objectNameText;
            QComboBox* groupComboBox;
            QLineEdit* newGroupText;
            QLabel* groupResultText;
    };

}
