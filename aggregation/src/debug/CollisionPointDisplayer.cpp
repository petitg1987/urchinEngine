#include <debug/CollisionPointDisplayer.h>

namespace urchin {

    CollisionPointDisplayer::CollisionPointDisplayer(PhysicsWorld& physicsWorld, Renderer3d& renderer3d) :
            physicsWorld(physicsWorld),
            renderer3d(renderer3d) {
        physicsWorld.createCollisionVisualizer();
    }

    CollisionPointDisplayer::~CollisionPointDisplayer() {
        clearDisplay();
    }

    void CollisionPointDisplayer::display() {
        clearDisplay();

        std::vector<Sphere<float>> contactSpheres;
        std::vector<ManifoldResult> manifoldResults = physicsWorld.getCollisionVisualizer().getManifoldResults();
        for (auto& manifoldResult : manifoldResults) {
            for (unsigned int j = 0; j < manifoldResult.getNumContactPoints(); ++j) {
                ManifoldContactPoint contactPoint = manifoldResult.getManifoldContactPoint(j);
                contactSpheres.emplace_back(0.05f, contactPoint.getPointOnObject2());
            }
        }

        if (!contactSpheres.empty()) {
            contactSpheresModel = std::make_shared<SphereModel>(contactSpheres, 7);
            contactSpheresModel->setPolygonMode(PolygonMode::FILL);
            renderer3d.getGeometryContainer().addGeometry(contactSpheresModel);
        }
    }

    void CollisionPointDisplayer::clearDisplay() {
        if (contactSpheresModel) {
            renderer3d.getGeometryContainer().removeGeometry(*contactSpheresModel);
            contactSpheresModel.reset();
        }
    }

}
