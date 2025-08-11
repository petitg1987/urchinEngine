#include <QtWidgets/QDialogButtonBox>

#include "panel/object/dialog/UpdateGroupDialog.h"
#include "panel/object/dialog/support/GroupHierarchyHelper.h"

namespace urchin {

    UpdateGroupDialog::UpdateGroupDialog(QWidget* parent, const std::vector<std::string>& defaultGroupHierarchy, const std::string& defaultGroupName, const ObjectController& objectController) :
            QDialog(parent),
            objectController(objectController),
            baseGroupComboBox(nullptr),
            groupNameText(nullptr) {
        this->setWindowTitle("Update Group");
        this->resize(530, 130);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupGroupFields(mainLayout, defaultGroupHierarchy, defaultGroupName);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void UpdateGroupDialog::setupGroupFields(QGridLayout* mainLayout, const std::vector<std::string>& defaultGroupHierarchy, const std::string& defaultGroupName) {
        auto* baseGroupLabel = new QLabel("Base Group:");
        mainLayout->addWidget(baseGroupLabel, 0, 0);

        std::vector<std::vector<std::string>> allGroupHierarchy = GroupHierarchyHelper::getAllGroupHierarchy(objectController);
        baseGroupComboBox = new QComboBox();
        mainLayout->addWidget(baseGroupComboBox, 0, 1);
        for (const std::vector<std::string>& groupHierarchy : allGroupHierarchy) {
            std::string groupHierarchyString = GroupHierarchyHelper::groupHierarchyToString(groupHierarchy);
            baseGroupComboBox->addItem(QString::fromStdString(groupHierarchyString), QVariant(QString::fromStdString(groupHierarchyString)));
        }
        baseGroupComboBox->setCurrentText(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(defaultGroupHierarchy)));

        auto* newGroupLabel = new QLabel("Group name:");
        mainLayout->addWidget(newGroupLabel, 1, 0);

        groupNameText = new QLineEdit();
        mainLayout->addWidget(groupNameText, 1, 1);
        groupNameText->setText(QString::fromStdString(defaultGroupName));

        auto* groupResultLabel = new QLabel("Result:");
        mainLayout->addWidget(groupResultLabel, 2, 0);

        groupResultText = new QLabel(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(getUpdatedGroupHierarchy())));
        mainLayout->addWidget(groupResultText, 2, 1);
        connect(baseGroupComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int){
            groupResultText->setText(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(getUpdatedGroupHierarchy())));
        });
        connect(groupNameText, &QLineEdit::textChanged, [&](const QString&){
            groupResultText->setText(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(getUpdatedGroupHierarchy())));
        });
    }

    std::vector<std::string> UpdateGroupDialog::getUpdatedGroupHierarchy() const {
        std::vector<std::string> groupHierarchy = GroupHierarchyHelper::stringToGroupHierarchy(baseGroupComboBox->currentData().toString().toStdString());
        if (!groupNameText->text().isEmpty()) {
            groupHierarchy.push_back(groupNameText->text().toStdString());
        }
        return groupHierarchy;
    }

}
