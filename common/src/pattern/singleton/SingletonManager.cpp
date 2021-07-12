#include <cassert>

#include <pattern/singleton/SingletonManager.h>

namespace urchin {

    //static
    std::map<std::string, std::unique_ptr<SingletonInterface>> SingletonManager::singletons;

    void SingletonManager::registerSingleton(const std::string& name, std::unique_ptr<SingletonInterface> ptr) {
        #ifndef NDEBUG
            assert(singletons.find(name) == singletons.end());
        #endif
        singletons.emplace(name, std::move(ptr));
    }

    void SingletonManager::destroyAllSingletons() {
        singletons.clear();
    }

}
