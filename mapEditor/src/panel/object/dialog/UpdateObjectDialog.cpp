#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include "panel/object/dialog/UpdateObjectDialog.h"
#include "panel/object/dialog/support/GroupHierarchyHelper.h"
#include "widget/style/LabelStyleHelper.h"

namespace urchin {

    UpdateObjectDialog::UpdateObjectDialog(QWidget* parent, std::string originalName, const std::vector<std::string>& originalGroupHierarchy, const ObjectController* objectController) :
            QDialog(parent),
            originalName(std::move(originalName)),
            objectController(objectController),
            updatedObjectNameLabel(nullptr),
            updatedObjectNameText(nullptr),
            updatedGroupComboBox(nullptr),
            newGroupText(nullptr) {
        this->setWindowTitle("Update Object");
        this->resize(530, 150);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);
        setupGroupFields(mainLayout, originalGroupHierarchy);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 3, 0, 1, 3);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void UpdateObjectDialog::setupNameFields(QGridLayout* mainLayout) {
        updatedObjectNameLabel = new QLabel("Updated Name:");
        mainLayout->addWidget(updatedObjectNameLabel, 0, 0);

        updatedObjectNameText = new QLineEdit();
        mainLayout->addWidget(updatedObjectNameText, 0, 1);
        updatedObjectNameText->setText(QString::fromStdString(originalName));
    }

    void UpdateObjectDialog::setupGroupFields(QGridLayout* mainLayout, const std::vector<std::string>& defaultGroupHierarchy) {
        auto* groupLabel = new QLabel("Updated Group*:");
        mainLayout->addWidget(groupLabel, 1, 0);

        std::string defaultGroupHierarchyString = StringUtil::join(defaultGroupHierarchy, GroupHierarchyHelper::GROUP_DELIMITER);
        std::vector<std::vector<std::string>> allGroupHierarchy = GroupHierarchyHelper::getAllGroupHierarchy(*objectController);

        updatedGroupComboBox = new QComboBox();
        mainLayout->addWidget(updatedGroupComboBox, 1, 1);
        updatedGroupComboBox->addItem("(root)", QVariant(""));
        for (const std::vector<std::string>& groupHierarchy : allGroupHierarchy) {
            std::string groupHierarchyString = StringUtil::join(groupHierarchy, GroupHierarchyHelper::GROUP_DELIMITER);
            updatedGroupComboBox->addItem(QString::fromStdString(groupHierarchyString), QVariant(QString::fromStdString(groupHierarchyString)));
            if (groupHierarchyString == defaultGroupHierarchyString) {
                updatedGroupComboBox->setCurrentIndex(updatedGroupComboBox->count() - 1);
            }
        }

        auto* newGroupLabel = new QLabel("New group:");
        mainLayout->addWidget(newGroupLabel, 2, 0);

        newGroupText = new QLineEdit();
        mainLayout->addWidget(newGroupText, 2, 1);
    }

    std::string UpdateObjectDialog::getUpdatedObjectName() const {
        return updatedObjectNameText->text().toStdString();
    }

    std::vector<std::string> UpdateObjectDialog::getUpdatedGroupHierarchy() const {
        std::vector<std::string> groupHierarchy = StringUtil::split(updatedGroupComboBox->currentData().toString().toStdString(), GroupHierarchyHelper::GROUP_DELIMITER);
        if (!newGroupText->text().isEmpty()) {
            groupHierarchy.push_back(newGroupText->text().toStdString());
        }
        return groupHierarchy;
    }

    void UpdateObjectDialog::done(int r) {
        if (Accepted == r) {
            bool hasError = false;

            LabelStyleHelper::applyNormalStyle(updatedObjectNameLabel);

            if (getUpdatedObjectName().empty()) {
                LabelStyleHelper::applyErrorStyle(updatedObjectNameLabel, "Object name is mandatory");
                hasError = true;
            } else if (getUpdatedObjectName() != originalName && isObjectEntityExist(getUpdatedObjectName())) {
                LabelStyleHelper::applyErrorStyle(updatedObjectNameLabel, "Object name is already used");
                hasError = true;
            }

            if (!hasError) {
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool UpdateObjectDialog::isObjectEntityExist(std::string_view name) const {
        std::list<const ObjectEntity*> objectEntities = objectController->getObjectEntities();
        return std::ranges::any_of(objectEntities, [name](const auto& so){ return so->getName() == name; });
    }

}
