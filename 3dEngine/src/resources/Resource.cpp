#include "resources/Resource.h"
#include "resources/ResourceManager.h"

namespace urchin {

    Resource::Resource() :
            refCount(1) {

    }

    Resource::~Resource() {
        if (!id.empty()) { //resource created with ResourceManager
            ResourceManager::instance()->removeResource(id);
        }
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

    unsigned int Resource::getRefCount() const {
        return refCount;
    }

    void Resource::addRef() {
        ++refCount;
    }

    void Resource::release() {
        assert(!name.empty()); //resource created with ResourceManager
        if (--refCount == 0) {
            delete this;
        }
    }

}
