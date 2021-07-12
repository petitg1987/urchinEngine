#pragma once

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>

#include <controller/ai/AIController.h>

namespace urchin {

    class AIPanelWidget : public QWidget {
        Q_OBJECT

        public:
            AIPanelWidget();
            ~AIPanelWidget() override = default;

            void load(AIController&);
            void unload();

        private:
            void setupNavMeshAgentBox(QVBoxLayout*);

            void setupNavMeshAgentDataFrom(const NavMeshAgent*);

            AIController* aiController;

            QDoubleSpinBox* agentHeight;
            QDoubleSpinBox* agentRadius;
            QDoubleSpinBox* maxSlope;
            QDoubleSpinBox* jumpDistance;

            bool disableAIEvent;

        private slots:
            void aiChanged();
    };

}
