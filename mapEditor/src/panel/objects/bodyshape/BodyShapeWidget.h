#pragma once

#include <string>
#include <memory>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>

#include <UrchinCommon.h>
#include <UrchinAggregation.h>
#include <UrchinPhysicsEngine.h>

namespace urchin {

    class BodyShapeWidget : public QWidget {
        Q_OBJECT

        public:
            static constexpr char SPHERE_SHAPE_LABEL[] = "Sphere";
            static constexpr char BOX_SHAPE_LABEL[] = "Box";
            static constexpr char CAPSULE_SHAPE_LABEL[] = "Capsule";
            static constexpr char CYLINDER_SHAPE_LABEL[] = "Cylinder";
            static constexpr char CONE_SHAPE_LABEL[] = "Cone";
            static constexpr char CONVEX_HULL_SHAPE_LABEL[] = "Convex Hull";
            static constexpr char COMPOUND_SHAPE_LABEL[] = "Compound Shape";

            explicit BodyShapeWidget(const ObjectEntity*);
            ~BodyShapeWidget() override = default;

            const ObjectEntity* getObjectEntity() const;
            virtual std::string getBodyShapeName() const = 0;
            std::unique_ptr<const CollisionShape3D> moveShape();

            void setupShapePropertiesFrom(const CollisionShape3D&);

        public slots:
            void updateBodyShape();

        protected:
            QGridLayout* mainLayout;
            bool disableShapeEvent;

            virtual void doSetupShapePropertiesFrom(const CollisionShape3D&) = 0;
            virtual std::unique_ptr<const CollisionShape3D> createBodyShape() const = 0;

        signals:
            void bodyShapeChange(std::unique_ptr<const CollisionShape3D>&);

        private:
            const ObjectEntity* objectEntity;

            std::unique_ptr<const CollisionShape3D> shape;
    };

}
