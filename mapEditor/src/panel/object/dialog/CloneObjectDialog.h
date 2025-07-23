#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include "controller/object/ObjectController.h"

namespace urchin {

    class CloneObjectDialog final : public QDialog {
            Q_OBJECT

        public:
            CloneObjectDialog(QWidget*, const std::string&, const ObjectController*);

            const std::string& getObjectName() const;

        private:
            void setupNameFields(QGridLayout*);

            void updateObjectName();

            void done(int) override;
            bool isObjectEntityExist(std::string_view) const;

            std::string proposedName;
            const ObjectController* objectController;

            QLabel* objectNameLabel;
            QLineEdit* objectNameText;

            std::string objectName;
    };

}
