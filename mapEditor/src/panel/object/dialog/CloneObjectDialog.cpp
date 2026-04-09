#include <QtWidgets/QDialogButtonBox>

#include "panel/object/dialog/CloneObjectDialog.h"
#include "panel/object/dialog/support/GroupHierarchyHelper.h"
#include "widget/style/LabelStyleHelper.h"
#include "widget/style/GroupBoxStyleHelper.h"
#include "controller/EntityControllerUtil.h"

namespace urchin {

    CloneObjectDialog::CloneObjectDialog(QWidget* parent, const std::string& originalName, std::vector<std::string> originalGroupHierarchy, const ObjectController& objectController) :
            QDialog(parent),
            proposedName(EntityControllerUtil::determineNewCloneName(originalName, originalGroupHierarchy, objectController.getObjectEntities())),
            originalGroupHierarchy(std::move(originalGroupHierarchy)),
            objectController(objectController),
            objectNameLabel(nullptr),
            objectNameText(nullptr) {
        this->setWindowTitle("Clone Object");
        this->resize(530, 220);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);
        setupGroupFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 3);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void CloneObjectDialog::setupNameFields(QGridLayout* mainLayout) {
        auto* generalGroupBox = new QGroupBox("General");
        mainLayout->addWidget(generalGroupBox, 0, 0);
        GroupBoxStyleHelper::applyDefaultStyle(generalGroupBox);

        auto* groupLayout = new QGridLayout(generalGroupBox);

        objectNameLabel = new QLabel("Name:");
        groupLayout->addWidget(objectNameLabel, 0, 0);

        objectNameText = new QLineEdit();
        groupLayout->addWidget(objectNameText, 0, 1);
        objectNameText->setText(QString::fromStdString(proposedName));
    }

    void CloneObjectDialog::setupGroupFields(QGridLayout* mainLayout) {
        auto* groupGroupBox = new QGroupBox("Group Hierarchy");
        mainLayout->addWidget(groupGroupBox, 1, 0);
        GroupBoxStyleHelper::applyDefaultStyle(groupGroupBox);
        auto* groupLayout = new QGridLayout(groupGroupBox);

        auto* groupLabel = new QLabel("Group:");
        groupLayout->addWidget(groupLabel, 0, 0);

        std::vector<std::vector<std::string>> allGroupHierarchy = GroupHierarchyHelper::getAllGroupHierarchy(objectController);
        groupComboBox = new QComboBox();
        groupLayout->addWidget(groupComboBox, 0, 1);
        for (const std::vector<std::string>& groupHierarchy : allGroupHierarchy) {
            std::string groupHierarchyString = GroupHierarchyHelper::groupHierarchyToString(groupHierarchy);
            groupComboBox->addItem(QString::fromStdString(groupHierarchyString), QVariant(QString::fromStdString(groupHierarchyString)));
        }
        groupComboBox->setCurrentText(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(originalGroupHierarchy)));

        auto* newGroupLabel = new QLabel("New group:");
        groupLayout->addWidget(newGroupLabel, 1, 0);

        newGroupText = new QLineEdit();
        groupLayout->addWidget(newGroupText, 1, 1);

        auto* groupResultLabel = new QLabel("Result:");
        groupLayout->addWidget(groupResultLabel, 2, 0);

        groupResultText = new QLabel(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(getGroupHierarchy())));
        groupLayout->addWidget(groupResultText, 2, 1);
        connect(groupComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int){
            groupResultText->setText(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(getGroupHierarchy())));
        });
        connect(newGroupText, &QLineEdit::textChanged, [&](const QString&){
            groupResultText->setText(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(getGroupHierarchy())));
        });
    }

    std::string CloneObjectDialog::getObjectName() const {
        return objectNameText->text().toStdString();
    }

    std::vector<std::string> CloneObjectDialog::getGroupHierarchy() const {
        std::vector<std::string> groupHierarchy = GroupHierarchyHelper::stringToGroupHierarchy(groupComboBox->currentData().toString().toStdString());
        if (!newGroupText->text().isEmpty()) {
            groupHierarchy.push_back(newGroupText->text().toStdString());
        }
        return groupHierarchy;
    }

    void CloneObjectDialog::done(int r) {
        if (Accepted == r) {
            bool hasError = false;

            LabelStyleHelper::resetErrorStyle(objectNameLabel);

            if (getObjectName().empty()) {
                LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is mandatory");
                hasError = true;
            } else if (isObjectEntityExist()) {
                LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name/group is duplicated");
                hasError = true;
            }

            if (!hasError) {
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool CloneObjectDialog::isObjectEntityExist() const {
        std::list<const ObjectEntity*> objectEntities = objectController.getObjectEntities();
        return std::ranges::any_of(objectEntities, [&](const auto& so){ return so->getName() == getObjectName() && so->getGroupHierarchy() == getGroupHierarchy(); });
    }

}
