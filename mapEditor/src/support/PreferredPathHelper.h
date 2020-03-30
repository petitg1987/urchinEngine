#ifndef URCHINENGINE_PREFERREDPATHHELPER_H
#define URCHINENGINE_PREFERREDPATHHELPER_H

#include "UrchinCommon.h"

namespace urchin
{

    class PreferredPathHelper : public Singleton<PreferredPathHelper>
    {
        public:
            friend class Singleton<PreferredPathHelper>;

            void savePreferredPath(const std::string &, const std::string &);
            std::string retrievePreferredPath(const std::string &) const;

        private:
            PreferredPathHelper();

            std::unique_ptr<PropertyFileHandler> propertyFileHandler;
    };

}

#endif
