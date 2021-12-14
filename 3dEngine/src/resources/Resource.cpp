#include <resources/Resource.h>
#include <resources/ResourceContainer.h>

namespace urchin {

    Resource::Resource() :
            permanent(false) {

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

    bool Resource::isPermanent() const {
        return permanent;
    }

    void Resource::setPermanent(bool permanent) {
        this->permanent = permanent;
    }

}
