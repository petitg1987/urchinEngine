#pragma once

#include <string>

namespace urchin {

    class Resource {
        public:
            Resource();
            virtual ~Resource();

            const std::string& getId() const;
            void setId(const std::string&);

            const std::string& getName() const;
            void setName(const std::string&);

            unsigned int getRefCount() const;
            void addRef();
            void release();

        private:
            std::string id;
            std::string name;
            unsigned int refCount;
    };

}
