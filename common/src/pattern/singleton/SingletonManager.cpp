#include <cassert>

#include <pattern/singleton/SingletonManager.h>

namespace urchin {

    //static
    std::map<std::string, SingletonInterface*> SingletonManager::singletons;

    void SingletonManager::registerSingleton(const std::string& name, SingletonInterface* ptr) {
        #ifndef NDEBUG
            assert(singletons.find(name) == singletons.end());
        #endif
        singletons[name] = ptr;
    }

    void SingletonManager::destroyAllSingletons() {
        for (auto& singleton : singletons) {
            delete singleton.second;
        }
        singletons.clear();
    }

}
