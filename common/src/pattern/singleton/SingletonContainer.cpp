#include <cassert>

#include <pattern/singleton/SingletonContainer.h>

namespace urchin {

    //static
    std::map<std::string, std::unique_ptr<SingletonInterface>> SingletonContainer::singletons;

    void SingletonContainer::registerSingleton(const std::string& name, std::unique_ptr<SingletonInterface> ptr) {
        #ifndef NDEBUG
            assert(singletons.find(name) == singletons.end());
        #endif
        singletons.emplace(name, std::move(ptr));
    }

    void SingletonContainer::destroyAllSingletons() {
        singletons.clear();
    }

}
