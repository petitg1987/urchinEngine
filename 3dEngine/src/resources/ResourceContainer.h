#pragma once

#include <map>
#include <string>
#include <UrchinCommon.h>

#include <resources/Resource.h>

namespace urchin {

    class ResourceContainer : public Singleton<ResourceContainer> {
        public:
            friend class Singleton<ResourceContainer>;

            ~ResourceContainer() override;

            template<class T> std::shared_ptr<T> getResource(const std::string&) const;
            void addResource(const std::shared_ptr<Resource>&);

        private:
            ResourceContainer();

            void cleanResources();

            std::map<std::string, std::weak_ptr<Resource>> resources;
    };

    #include "ResourceContainer.inl"

}
