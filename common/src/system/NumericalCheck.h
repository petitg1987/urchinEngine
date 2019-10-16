#ifndef URCHINENGINE_NUMERICALCHECK_H
#define URCHINENGINE_NUMERICALCHECK_H

#include "pattern/singleton/Singleton.h"

namespace urchin
{

    class NumericalCheck : public Singleton<NumericalCheck>
    {
        public:
            friend class Singleton<NumericalCheck>;

            void perform() const;

        private:
            NumericalCheck() = default;
            ~NumericalCheck() override = default;
    };

}

#endif
