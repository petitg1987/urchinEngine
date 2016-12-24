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

		//TODO load AI
		agentHeight->setValue(1.9);
		agentRadius->setValue(0.3);
	}

	void AIControllerWidget::unload()
	{
		//TODO unload AI

		aiController = nullptr;
	}

	void AIControllerWidget::setupGenerateNavMeshBox(QVBoxLayout *mainLayout)
	{
		QGroupBox *generateNavMeshGroupBox = new QGroupBox("Generate Nav Mesh");
		mainLayout->addWidget(generateNavMeshGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(generateNavMeshGroupBox);

		QGridLayout *generateNavMeshLayout = new QGridLayout(generateNavMeshGroupBox);
		generateNavMeshLayout->setAlignment(Qt::AlignLeft);

		QLabel *agentHeightLabel= new QLabel("Agent Height:");
		generateNavMeshLayout->addWidget(agentHeightLabel, 0, 0);

		agentHeight = new QDoubleSpinBox();
		generateNavMeshLayout->addWidget(agentHeight, 0, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(agentHeight);
		agentHeight->setMinimum(0.0);

		QLabel *agentRadiusLabel= new QLabel("Agent Radius:");
		generateNavMeshLayout->addWidget(agentRadiusLabel, 1, 0);

		agentRadius = new QDoubleSpinBox();
		generateNavMeshLayout->addWidget(agentRadius, 1, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(agentRadius);
		agentRadius->setMinimum(0.0);

		generateNavMeshButton = new QPushButton("Generate");
		generateNavMeshLayout->addWidget(generateNavMeshButton, 2, 0, 1, 2);
		ButtonStyleHelper::applyNormalStyle(generateNavMeshButton);
		connect(generateNavMeshButton, SIGNAL(clicked()), this, SLOT(generateNavMesh()));
	}

	void AIControllerWidget::generateNavMesh()
	{
		aiController->generateNavMesh(agentHeight->value(), agentRadius->value());
	}

}
