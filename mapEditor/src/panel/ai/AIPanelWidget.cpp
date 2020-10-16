#include <QtWidgets/QGroupBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

#include "AIPanelWidget.h"
#include "widget/style/GroupBoxStyleHelper.h"
#include "widget/style/SpinBoxStyleHelper.h"

namespace urchin {

    AIPanelWidget::AIPanelWidget() :
            aiController(nullptr),
            agentHeight(nullptr),
            agentRadius(nullptr),
            maxSlope(nullptr),
            jumpDistance(nullptr),
            disableAIEvent(false) {
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        setupNavMeshAgentBox(mainLayout);
    }

    void AIPanelWidget::load(AIController *aiController) {
        this->aiController = aiController;

        std::shared_ptr<NavMeshAgent> navMeshAgent = aiController->getSceneAI()->getNavMeshAgent();
        setupNavMeshAgentDataFrom(navMeshAgent);
    }

    void AIPanelWidget::unload() {
        aiController = nullptr;
    }

    void AIPanelWidget::setupNavMeshAgentBox(QVBoxLayout *mainLayout) {
        auto *navMeshAgentGroupBox = new QGroupBox("Nav Mesh Agent");
        mainLayout->addWidget(navMeshAgentGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(navMeshAgentGroupBox);
        navMeshAgentGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto *navMeshAgentLayout = new QGridLayout(navMeshAgentGroupBox);

        auto *agentHeightLabel = new QLabel("Agent Height:");
        navMeshAgentLayout->addWidget(agentHeightLabel, 0, 0);

        agentHeight = new QDoubleSpinBox();
        navMeshAgentLayout->addWidget(agentHeight, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(agentHeight);
        agentHeight->setMinimum(0.0);
        connect(agentHeight, SIGNAL(valueChanged(double)), this, SLOT(aiChanged()));

        auto *agentRadiusLabel = new QLabel("Agent Radius:");
        navMeshAgentLayout->addWidget(agentRadiusLabel, 0, 2);

        agentRadius = new QDoubleSpinBox();
        navMeshAgentLayout->addWidget(agentRadius, 0, 3);
        SpinBoxStyleHelper::applyDefaultStyleOn(agentRadius);
        agentRadius->setMinimum(0.0);
        connect(agentRadius, SIGNAL(valueChanged(double)), this, SLOT(aiChanged()));

        auto *maxSlopeLabel = new QLabel("Max Slope (°):");
        navMeshAgentLayout->addWidget(maxSlopeLabel, 1, 0);

        maxSlope = new QDoubleSpinBox();
        navMeshAgentLayout->addWidget(maxSlope, 1, 1);
        SpinBoxStyleHelper::applyAngleStyleOn(maxSlope);
        maxSlope->setMinimum(5.0);
        maxSlope->setMaximum(85.0);
        connect(maxSlope, SIGNAL(valueChanged(double)), this, SLOT(aiChanged()));

        auto *jumpDistanceLabel = new QLabel("Jump distance:");
        navMeshAgentLayout->addWidget(jumpDistanceLabel, 1, 2);

        jumpDistance = new QDoubleSpinBox();
        navMeshAgentLayout->addWidget(jumpDistance, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyleOn(jumpDistance);
        jumpDistance->setMinimum(0.0);
        connect(jumpDistance, SIGNAL(valueChanged(double)), this, SLOT(aiChanged()));
    }

    void AIPanelWidget::aiChanged() {
        if (!disableAIEvent) {
            std::shared_ptr<NavMeshAgent> navMeshAgent = std::make_shared<NavMeshAgent>(static_cast<float>(agentHeight->value()), static_cast<float>(agentRadius->value()));
            navMeshAgent->setMaxSlope(AngleConverter<float>::toRadian(maxSlope->value()));
            navMeshAgent->setJumpDistance(static_cast<float>(jumpDistance->value()));

            aiController->updateSceneAI(navMeshAgent);
        }
    }

    void AIPanelWidget::setupNavMeshAgentDataFrom(const std::shared_ptr<NavMeshAgent> &navMeshAgent) {
        disableAIEvent = true;
        agentHeight->setValue(navMeshAgent->getAgentHeight());
        agentRadius->setValue(navMeshAgent->getAgentRadius());
        maxSlope->setValue(AngleConverter<float>::toDegree(navMeshAgent->getMaxSlope()));
        jumpDistance->setValue(navMeshAgent->getJumpDistance());
        disableAIEvent = false;
    }

}
