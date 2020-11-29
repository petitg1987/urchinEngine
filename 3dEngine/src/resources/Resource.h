#ifndef URCHINENGINE_RESOURCE_H
#define URCHINENGINE_RESOURCE_H

#include <string>

namespace urchin {

    class Resource {
        public:
            Resource();
            virtual ~Resource();

            const std::string& getName() const;
            void setName(const std::string&);

            unsigned int getRefCount() const;
            void addRef();
            void release();

        private:
            std::string name;
            unsigned int refCount;
    };

}

#endif
