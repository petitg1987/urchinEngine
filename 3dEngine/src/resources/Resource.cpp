#include <resources/Resource.h>
#include <resources/ResourceContainer.h>

namespace urchin {

    Resource::Resource() :
            deathTime(0.0f) {

    }

    const std::string& Resource::getId() const {
        return id;
    }

    void Resource::setId(const std::string& id) {
        this->id = id;
    }

    const std::string& Resource::getName() const {
        return name;
    }

    void Resource::setName(const std::string& name) {
        this->name = name;
    }

    void Resource::resetDeathTime() {
        deathTime = 0.0f;
    }

    void Resource::increaseDeathTime(float dt) {
        deathTime += dt;
    }

    float Resource::getDeathTime() const {
        return deathTime;
    }

}
