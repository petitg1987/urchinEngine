#include <display/CollisionPointDisplayer.h>

namespace urchin {

    CollisionPointDisplayer::CollisionPointDisplayer(PhysicsWorld& physicsWorld, Renderer3d* renderer3d) :
            physicsWorld(physicsWorld),
            renderer3d(renderer3d) {
        physicsWorld.createCollisionVisualizer();
    }

    CollisionPointDisplayer::~CollisionPointDisplayer() {
        clearDisplay();
    }

    void CollisionPointDisplayer::display() {
        clearDisplay();
        
        std::vector<ManifoldResult> manifoldResults = physicsWorld.getCollisionVisualizer().getManifoldResults();
        for (auto& manifoldResult : manifoldResults) {
            for (unsigned int j = 0; j < manifoldResult.getNumContactPoints(); ++j) {
                ManifoldContactPoint contactPoint = manifoldResult.getManifoldContactPoint(j);

                auto sphereModel = std::make_shared<SphereModel>(Sphere<float>(0.05f, contactPoint.getPointOnObject2()), 7);
                contactPointModels.push_back(sphereModel);
                renderer3d->getGeometryContainer().addGeometry(sphereModel);
            }
        }
    }

    void CollisionPointDisplayer::clearDisplay() {
        for (const auto& contactPointModel : contactPointModels) {
            renderer3d->getGeometryContainer().removeGeometry(*contactPointModel);
        }
        contactPointModels.clear();
    }
    
}
