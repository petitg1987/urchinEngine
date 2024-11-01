#include <utility>

#include <system/thread/LockById.h>

namespace urchin {

    //static
    std::map<std::string, std::shared_ptr<LockById>, std::less<>> LockById::instances;

    LockById::LockById(std::string instanceName) :
            instanceName(std::move(instanceName)) {

    }

    std::shared_ptr<LockById> LockById::getInstance(const std::string& instanceName) {
        auto instanceIt = instances.find(instanceName);
        if (instanceIt != instances.end()) {
            return instanceIt->second;
        }

        auto lockById = std::make_shared<LockById>(instanceName);
        instances.try_emplace(instanceName, lockById);
        return lockById;
    }

    void LockById::lock(uint_fast32_t id) {
        std::mutex* mutex;
        {
            std::scoped_lock lock(accessMutex);
            mutex = &mutexById[id];
        }

        mutex->lock();
    }

    void LockById::unlock(uint_fast32_t id) {
        std::scoped_lock lock(accessMutex);
        mutexById.find(id)->second.unlock();
    }

}
