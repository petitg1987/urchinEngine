#ifndef URCHINENGINE_SCOPELOCKBYID_H
#define URCHINENGINE_SCOPELOCKBYID_H

#include <memory>

#include "system/thread/LockById.h"

namespace urchin {

    class ScopeLockById {
        public:
            ScopeLockById(const std::shared_ptr<LockById>&, uint_fast32_t);
            ~ScopeLockById();

        private:
            std::shared_ptr<LockById> lockById;
            uint_fast32_t id;
    };

}

#endif
