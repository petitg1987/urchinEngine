#include <QtWidgets/QGroupBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

#include "AIControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/ButtonStyleHelper.h"

namespace urchin
{

	AIControllerWidget::AIControllerWidget() :
			aiController(nullptr)
	{
		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		mainLayout->setAlignment(Qt::AlignTop);
		mainLayout->setContentsMargins(1, 1, 1, 1);

		setupGenerateNavMeshBox(mainLayout);
	}

	AIControllerWidget::~AIControllerWidget()
	{

	}

	void AIControllerWidget::load(AIController *aiController)
	{
		this->aiController = aiController;

		//TODO load AI from map file
		agentHeight->setValue(1.9);
		agentRadius->setValue(0.25);
		maxSlope->setValue(45.0);
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

		QGridLayout *generateNavMeshLayout = new QGridLayout(generateNavMeshGroupBox);

		QLabel *agentHeightLabel= new QLabel("Agent Height:");
		generateNavMeshLayout->addWidget(agentHeightLabel, 0, 0);

		agentHeight = new QDoubleSpinBox();
		generateNavMeshLayout->addWidget(agentHeight, 0, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(agentHeight);
		agentHeight->setMinimum(0.0);

		QLabel *agentRadiusLabel= new QLabel("Agent Radius:");
		generateNavMeshLayout->addWidget(agentRadiusLabel, 0, 2);

		agentRadius = new QDoubleSpinBox();
		generateNavMeshLayout->addWidget(agentRadius, 0, 3);
		SpinBoxStyleHelper::applyDefaultStyleOn(agentRadius);
		agentRadius->setMinimum(0.0);

		QLabel *maxSlopeLabel= new QLabel("Max Slope (°):");
		generateNavMeshLayout->addWidget(maxSlopeLabel, 1, 0);

		maxSlope = new QDoubleSpinBox();
		generateNavMeshLayout->addWidget(maxSlope, 1, 1);
		SpinBoxStyleHelper::applyAngleStyleOn(maxSlope);
		maxSlope->setMinimum(5.0);
		maxSlope->setMaximum(85.0);

		generateNavMeshButton = new QPushButton("Generate");
		generateNavMeshLayout->addWidget(generateNavMeshButton, 2, 0, 1, 4);
		ButtonStyleHelper::applyNormalStyle(generateNavMeshButton);
		connect(generateNavMeshButton, SIGNAL(clicked()), this, SLOT(generateNavMesh()));
	}

	void AIControllerWidget::generateNavMesh()
	{
		aiController->generateNavMesh(agentHeight->value(), agentRadius->value(), maxSlope->value());
	}

}
