#include <resources/Resource.h>
#include <resources/ResourceContainer.h>

namespace urchin {

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

}
