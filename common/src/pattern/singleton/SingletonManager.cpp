#include <cassert>

#include <pattern/singleton/SingletonManager.h>

namespace urchin {

    //static
    std::map<std::string, SingletonInterface*> SingletonManager::singletons;

    void* SingletonManager::getSingleton(const std::string& name) {
        auto it = singletons.find(name);
        if (it == singletons.end()) {
            return nullptr;
        }
        return it->second;
    }

    void SingletonManager::addSingleton(const std::string& name, SingletonInterface* ptr) {
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
