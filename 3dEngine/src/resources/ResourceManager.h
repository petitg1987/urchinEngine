#ifndef URCHINENGINE_RESOURCEMANAGER_H
#define URCHINENGINE_RESOURCEMANAGER_H

#include <map>
#include <string>
#include "UrchinCommon.h"

#include "resources/Resource.h"

namespace urchin {

    class ResourceManager : public Singleton<ResourceManager> {
        public:
            friend class Singleton<ResourceManager>;

            template<class T> T* getResource(const std::string&) const;
            void addResource(const std::string&, Resource*);
            void removeResource(const std::string&);

        private:
            ResourceManager();
            ~ResourceManager() override;

            std::map<std::string, Resource*> mResources;
    };

    #include "ResourceManager.inl"

}

#endif
