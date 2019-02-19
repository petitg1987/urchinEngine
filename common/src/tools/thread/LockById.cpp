#include "LockById.h"

namespace urchin
{

    //static
    std::map<std::string, std::shared_ptr<LockById>> LockById::instances;

    LockById::LockById(const std::string &instanceName) :
            instanceName(instanceName)
    {

    }

    std::shared_ptr<LockById> LockById::getInstance(const std::string &instanceName)
    {
        auto instanceIt = instances.find(instanceName);
        if(instanceIt!=instances.end())
        {
            return instanceIt->second;
        }

        auto lockById = std::make_shared<LockById>(instanceName);
        instances.insert(std::pair<std::string, std::shared_ptr<LockById>>(instanceName, lockById));
        return lockById;
    }

    void LockById::lock(uint_fast32_t id)
    {
        accessMutex.lock();
        std::mutex &mutex = mutexById[id];
        accessMutex.unlock();

        mutex.lock();
    }

    void LockById::unlock(uint_fast32_t id)
    {
        std::lock_guard<std::mutex> lock(accessMutex);

        mutexById.find(id)->second.unlock();
    }

}
