#ifndef URCHINENGINE_CHECK_H
#define URCHINENGINE_CHECK_H

#include "pattern/singleton/Singleton.h"

namespace urchin
{

    class Check : public Singleton<Check>
    {
        public:
            friend class Singleton<Check>;

            bool additionalChecksEnable();

        private:
            Check();
            ~Check() override = default;

            bool isAdditionalCheckEnable;
    };

}

#endif
