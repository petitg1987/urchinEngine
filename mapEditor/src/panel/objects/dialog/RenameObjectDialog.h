#pragma once

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include <UrchinAggregation.h>
#include <controller/objects/ObjectController.h>

namespace urchin {

    class RenameObjectDialog : public QDialog {
        Q_OBJECT

        public:
            RenameObjectDialog(QWidget*, std::string, const ObjectController*);

            std::string getObjectName() const;

        private:
            void setupNameFields(QGridLayout*);

            void updateObjectName();

            void done(int) override;
            bool isObjectEntityExist(std::string_view) const;

            std::string originalName;
            const ObjectController* objectController;

            QLabel* objectNameLabel;
            QLineEdit* objectNameText;

            std::string objectName;
    };

}
