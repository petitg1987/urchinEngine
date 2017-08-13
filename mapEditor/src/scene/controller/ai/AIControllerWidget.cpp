#include <QtWidgets/QGroupBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

#include "AIControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"

namespace urchin
{

	AIControllerWidget::AIControllerWidget() :
			aiController(nullptr)
	{
		auto *mainLayout = new QVBoxLayout(this);
		mainLayout->setAlignment(Qt::AlignTop);
		mainLayout->setContentsMargins(1, 1, 1, 1);

		setupGenerateNavMeshBox(mainLayout);
	}

	void AIControllerWidget::load(AIController *aiController)
	{
		this->aiController = aiController;

		std::shared_ptr<NavMeshConfig> navMeshConfig = aiController->getSceneAI()->getNavMeshConfig();

		agentHeight->setValue(navMeshConfig->getAgent().getAgentHeight());
		agentRadius->setValue(navMeshConfig->getAgent().getAgentRadius());
		maxSlope->setValue(AngleConverter<float>::toDegree(navMeshConfig->getMaxSlope()));
	}

	void AIControllerWidget::unload()
	{
		aiController = nullptr;
	}

	void AIControllerWidget::setupGenerateNavMeshBox(QVBoxLayout *mainLayout)
	{
		QGroupBox *generateNavMeshGroupBox = new QGroupBox("Generate Nav Mesh");
		mainLayout->addWidget(generateNavMeshGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(generateNavMeshGroupBox);
		generateNavMeshGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

		auto *generateNavMeshLayout = new QGridLayout(generateNavMeshGroupBox);

		QLabel *agentHeightLabel= new QLabel("Agent Height:");
		generateNavMeshLayout->addWidget(agentHeightLabel, 0, 0);

		agentHeight = new QDoubleSpinBox();
		generateNavMeshLayout->addWidget(agentHeight, 0, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(agentHeight);
		agentHeight->setMinimum(0.0);
		connect(agentHeight, SIGNAL(valueChanged(double)), this, SLOT(navMeshConfigChanged()));

		QLabel *agentRadiusLabel= new QLabel("Agent Radius:");
		generateNavMeshLayout->addWidget(agentRadiusLabel, 0, 2);

		agentRadius = new QDoubleSpinBox();
		generateNavMeshLayout->addWidget(agentRadius, 0, 3);
		SpinBoxStyleHelper::applyDefaultStyleOn(agentRadius);
		agentRadius->setMinimum(0.0);
		connect(agentRadius, SIGNAL(valueChanged(double)), this, SLOT(navMeshConfigChanged()));

		QLabel *maxSlopeLabel= new QLabel("Max Slope (°):");
		generateNavMeshLayout->addWidget(maxSlopeLabel, 1, 0);

		maxSlope = new QDoubleSpinBox();
		generateNavMeshLayout->addWidget(maxSlope, 1, 1);
		SpinBoxStyleHelper::applyAngleStyleOn(maxSlope);
		maxSlope->setMinimum(5.0);
		maxSlope->setMaximum(85.0);
		connect(maxSlope, SIGNAL(valueChanged(double)), this, SLOT(navMeshConfigChanged()));
	}

	void AIControllerWidget::navMeshConfigChanged()
	{
		std::shared_ptr<NavMeshConfig> navMeshConfig = std::make_shared<NavMeshConfig>(NavMeshAgent(agentHeight->value(), agentRadius->value()));
		navMeshConfig->setMaxSlope(AngleConverter<float>::toRadian(maxSlope->value()));

		aiController->updateNavMeshConfig(navMeshConfig);
	}

}
