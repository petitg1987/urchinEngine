#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include "panel/object/dialog/AddObjectDialog.h"
#include "panel/object/dialog/support/GroupHierarchyHelper.h"
#include "widget/style/LabelStyleHelper.h"
#include "widget/style/ButtonStyleHelper.h"
#include "widget/style/GroupBoxStyleHelper.h"

namespace urchin {

    AddObjectDialog::AddObjectDialog(QWidget* parent, const std::vector<std::string>& defaultGroupHierarchy, const ObjectController* objectController) :
            QDialog(parent),
            objectController(objectController),
            objectNameLabel(nullptr),
            objectNameText(nullptr),
            groupComboBox(nullptr),
            newGroupText(nullptr),
            objectEntity(nullptr) {
        this->setWindowTitle("Add Object");
        this->resize(530, 220);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameField(mainLayout);
        setupGroupFields(mainLayout, defaultGroupHierarchy);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 3, 0);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void AddObjectDialog::setupNameField(QGridLayout* mainLayout) {
        auto* generalGroupBox = new QGroupBox("General");
        mainLayout->addWidget(generalGroupBox, 0, 0);
        GroupBoxStyleHelper::applyNormalStyle(generalGroupBox);

        auto* groupLayout = new QGridLayout(generalGroupBox);

        objectNameLabel = new QLabel("Object Name:");
        groupLayout->addWidget(objectNameLabel, 0, 0);

        objectNameText = new QLineEdit();
        groupLayout->addWidget(objectNameText, 0, 1);
    }

    void AddObjectDialog::setupGroupFields(QGridLayout* mainLayout, const std::vector<std::string>& defaultGroupHierarchy) {
        auto* groupGroupBox = new QGroupBox("Group Hierarchy");
        mainLayout->addWidget(groupGroupBox, 1, 0);
        GroupBoxStyleHelper::applyNormalStyle(groupGroupBox);

        auto* groupLayout = new QGridLayout(groupGroupBox);

        auto* groupLabel = new QLabel("Group:");
        groupLayout->addWidget(groupLabel, 0, 0);

        std::vector<std::vector<std::string>> allGroupHierarchy = GroupHierarchyHelper::getAllGroupHierarchy(*objectController);
        groupComboBox = new QComboBox();
        groupLayout->addWidget(groupComboBox, 0, 1);
        for (const std::vector<std::string>& groupHierarchy : allGroupHierarchy) {
            std::string groupHierarchyString = GroupHierarchyHelper::groupHierarchyToString(groupHierarchy);
            groupComboBox->addItem(QString::fromStdString(groupHierarchyString), QVariant(QString::fromStdString(groupHierarchyString)));
        }
        groupComboBox->setCurrentText(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(defaultGroupHierarchy)));

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

    std::vector<std::string> AddObjectDialog::getGroupHierarchy() const {
        std::vector<std::string> groupHierarchy = GroupHierarchyHelper::stringToGroupHierarchy(groupComboBox->currentData().toString().toStdString());
        if (!newGroupText->text().isEmpty()) {
            groupHierarchy.push_back(newGroupText->text().toStdString());
        }
        return groupHierarchy;
    }

    int AddObjectDialog::buildObjectEntity(int result) {
        objectEntity = std::make_unique<ObjectEntity>();
        objectEntity->setModel(Model::fromMeshesFile(""));
        objectEntity->setName(objectNameText->text().toStdString());
        objectEntity->setGroupHierarchy(getGroupHierarchy());
        return result;
    }

    std::unique_ptr<ObjectEntity> AddObjectDialog::moveObjectEntity() {
        return std::move(objectEntity);
    }

    void AddObjectDialog::done(int r) {
        if (Accepted == r) {
            bool hasError = false;

            LabelStyleHelper::applyNormalStyle(objectNameLabel);

            if (objectNameText->text().isEmpty()) {
                LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is mandatory");
                hasError = true;
            } else if (isObjectEntityExist(objectNameText->text().toStdString())) {
                LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is already used");
                hasError = true;
            }

            if (!hasError) {
                r = buildObjectEntity(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool AddObjectDialog::isObjectEntityExist(std::string_view name) const {
        std::list<const ObjectEntity*> objectEntities = objectController->getObjectEntities();
        return std::ranges::any_of(objectEntities, [name](const auto& so){ return so->getName() == name; });
    }

}
