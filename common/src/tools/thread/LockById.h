#ifndef URCHINENGINE_LOCKBYID_H
#define URCHINENGINE_LOCKBYID_H

#include <map>
#include <mutex>
#include <memory>
#include <string>

namespace urchin {

    class LockById {
        public:
            explicit LockById(std::string );

            static std::shared_ptr<LockById> getInstance(const std::string &);

            void lock(uint_fast32_t);
            void unlock(uint_fast32_t);

        private:
            static std::map<std::string, std::shared_ptr<LockById>> instances;

            std::string instanceName;

            std::mutex accessMutex;
            std::map<uint_fast32_t, std::mutex> mutexById;

    };

}

#endif
