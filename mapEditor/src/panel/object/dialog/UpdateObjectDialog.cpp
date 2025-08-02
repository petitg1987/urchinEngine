#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include "panel/object/dialog/UpdateObjectDialog.h"
#include "panel/object/dialog/support/GroupHierarchyHelper.h"
#include "widget/style/GroupBoxStyleHelper.h"
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
        this->resize(530, 220);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);
        setupGroupFields(mainLayout, originalGroupHierarchy);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 3, 0);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void UpdateObjectDialog::setupNameFields(QGridLayout* mainLayout) {
        auto* generalGroupBox = new QGroupBox("General");
        mainLayout->addWidget(generalGroupBox, 0, 0);
        GroupBoxStyleHelper::applyNormalStyle(generalGroupBox);

        auto* groupLayout = new QGridLayout(generalGroupBox);

        updatedObjectNameLabel = new QLabel("Updated Name:");
        groupLayout->addWidget(updatedObjectNameLabel, 0, 0);

        updatedObjectNameText = new QLineEdit();
        groupLayout->addWidget(updatedObjectNameText, 0, 1);
        updatedObjectNameText->setText(QString::fromStdString(originalName));
    }

    void UpdateObjectDialog::setupGroupFields(QGridLayout* mainLayout, const std::vector<std::string>& defaultGroupHierarchy) {
        auto* groupGroupBox = new QGroupBox("Group Hierarchy");
        mainLayout->addWidget(groupGroupBox, 1, 0);
        GroupBoxStyleHelper::applyNormalStyle(groupGroupBox);
        auto* groupLayout = new QGridLayout(groupGroupBox);

        auto* groupLabel = new QLabel("Updated Group:");
        groupLayout->addWidget(groupLabel, 0, 0);

        std::vector<std::vector<std::string>> allGroupHierarchy = GroupHierarchyHelper::getAllGroupHierarchy(*objectController);
        updatedGroupComboBox = new QComboBox();
        groupLayout->addWidget(updatedGroupComboBox, 0, 1);
        for (const std::vector<std::string>& groupHierarchy : allGroupHierarchy) {
            std::string groupHierarchyString = GroupHierarchyHelper::groupHierarchyToString(groupHierarchy);
            updatedGroupComboBox->addItem(QString::fromStdString(groupHierarchyString), QVariant(QString::fromStdString(groupHierarchyString)));
        }
        updatedGroupComboBox->setCurrentText(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(defaultGroupHierarchy)));

        auto* newGroupLabel = new QLabel("New group:");
        groupLayout->addWidget(newGroupLabel, 1, 0);

        newGroupText = new QLineEdit();
        groupLayout->addWidget(newGroupText, 1, 1);

        auto* groupResultLabel = new QLabel("Result:");
        groupLayout->addWidget(groupResultLabel, 2, 0);

        groupResultText = new QLabel(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(getUpdatedGroupHierarchy())));
        groupLayout->addWidget(groupResultText, 2, 1);
        connect(updatedGroupComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int){
            groupResultText->setText(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(getUpdatedGroupHierarchy())));
        });
        connect(newGroupText, &QLineEdit::textChanged, [&](const QString&){
            groupResultText->setText(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(getUpdatedGroupHierarchy())));
        });
    }

    std::string UpdateObjectDialog::getUpdatedObjectName() const {
        return updatedObjectNameText->text().toStdString();
    }

    std::vector<std::string> UpdateObjectDialog::getUpdatedGroupHierarchy() const {
        std::vector<std::string> groupHierarchy = GroupHierarchyHelper::stringToGroupHierarchy(updatedGroupComboBox->currentData().toString().toStdString());
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
