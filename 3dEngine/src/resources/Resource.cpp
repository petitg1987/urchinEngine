#include "resources/Resource.h"
#include "resources/ResourceManager.h"

namespace urchin {

    Resource::Resource() :
            refCount(1) {

    }

    Resource::~Resource() {
        if (!name.empty()) { //resource created with ResourceManager
            ResourceManager::instance()->removeResource(name);
        }
    }

    const std::string& Resource::getName() const {
        return name;
    }

    void Resource::setName(const std::string& nameU) {
        name = nameU;
    }

    unsigned int Resource::getRefCount() const {
        return refCount;
    }

    void Resource::addRef() {
        ++refCount;
    }

    void Resource::release() {
        assert(!name.empty()); //resource created with ResourceManager
        if (--refCount==0) {
            delete this;
        }
    }

}
