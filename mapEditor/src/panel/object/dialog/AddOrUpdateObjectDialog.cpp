#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include "panel/object/dialog/AddOrUpdateObjectDialog.h"
#include "panel/object/dialog/support/GroupHierarchyHelper.h"
#include "widget/style/GroupBoxStyleHelper.h"
#include "widget/style/LabelStyleHelper.h"

namespace urchin {

    AddOrUpdateObjectDialog::AddOrUpdateObjectDialog(QWidget* parent, const ObjectController& objectController, std::string originalName) :
            QDialog(parent),
            originalName(std::move(originalName)),
            objectController(objectController),
            defaultLightMask(std::numeric_limits<uint8_t>::max()),
            objectNameLabel(nullptr),
            objectNameText(nullptr),
            groupComboBox(nullptr),
            newGroupText(nullptr) {
        this->setWindowTitle("Add/Update Object");
        this->resize(530, 220);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);
        setupGroupFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 3, 0);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void AddOrUpdateObjectDialog::setupNameFields(QGridLayout* mainLayout) {
        auto* generalGroupBox = new QGroupBox("General");
        mainLayout->addWidget(generalGroupBox, 0, 0);
        GroupBoxStyleHelper::applyDefaultStyle(generalGroupBox);

        auto* groupLayout = new QGridLayout(generalGroupBox);

        objectNameLabel = new QLabel("Name:");
        groupLayout->addWidget(objectNameLabel, 0, 0);

        objectNameText = new QLineEdit();
        groupLayout->addWidget(objectNameText, 0, 1);
    }

    void AddOrUpdateObjectDialog::setupGroupFields(QGridLayout* mainLayout) {
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

    void AddOrUpdateObjectDialog::setDefaultValues(const std::string& defaultName, const std::vector<std::string>& defaultGroupHierarchy, uint8_t defaultLightMask) {
        objectNameText->setText(QString::fromStdString(defaultName));
        groupComboBox->setCurrentText(QString::fromStdString(GroupHierarchyHelper::groupHierarchyToString(defaultGroupHierarchy)));
        this->defaultLightMask = defaultLightMask;
    }

    std::unique_ptr<ObjectEntity> AddOrUpdateObjectDialog::getNewObjectEntity() const {
        auto objectEntity = std::make_unique<ObjectEntity>();
        objectEntity->setModel(Model::fromMeshesFile(""));
        objectEntity->getModel()->setLightMask(defaultLightMask);
        objectEntity->setName(objectNameText->text().toStdString());
        objectEntity->setGroupHierarchy(getGroupHierarchy());
        return objectEntity;
    }

    std::string AddOrUpdateObjectDialog::getObjectName() const {
        return objectNameText->text().toStdString();
    }

    std::vector<std::string> AddOrUpdateObjectDialog::getGroupHierarchy() const {
        std::vector<std::string> groupHierarchy = GroupHierarchyHelper::stringToGroupHierarchy(groupComboBox->currentData().toString().toStdString());
        if (!newGroupText->text().isEmpty()) {
            groupHierarchy.push_back(newGroupText->text().toStdString());
        }
        return groupHierarchy;
    }

    void AddOrUpdateObjectDialog::done(int r) {
        if (Accepted == r) {
            bool hasError = false;

            LabelStyleHelper::resetErrorStyle(objectNameLabel);

            if (getObjectName().empty()) {
                LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is mandatory");
                hasError = true;
            } else if (getObjectName() != originalName && isObjectEntityExist()) {
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

    bool AddOrUpdateObjectDialog::isObjectEntityExist() const {
        std::list<const ObjectEntity*> objectEntities = objectController.getObjectEntities();
        return std::ranges::any_of(objectEntities, [&](const auto& oe){ return oe->getName() == getObjectName() && oe->getGroupHierarchy() == getGroupHierarchy(); });
    }

}
