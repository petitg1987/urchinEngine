#pragma once

#include <map>
#include <string>
#include <UrchinCommon.h>

#include <resources/Resource.h>

namespace urchin {

    class ResourceManager : public Singleton<ResourceManager> {
        public:
            friend class Singleton<ResourceManager>;

            ~ResourceManager() override;

            template<class T> std::shared_ptr<T> getResource(const std::string&) const;
            void addResource(const std::shared_ptr<Resource>&);

        private:
            ResourceManager();

            void cleanResources();

            std::map<std::string, std::weak_ptr<Resource>> resources;
    };

    #include "ResourceManager.inl"

}
