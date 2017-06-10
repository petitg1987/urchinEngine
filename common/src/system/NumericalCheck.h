#ifndef ENGINE_NUMERICALCHECK_H
#define ENGINE_NUMERICALCHECK_H

#include "pattern/singleton/Singleton.h"

namespace urchin
{

    class NumericalCheck : public Singleton<NumericalCheck>
    {
        public:
            friend class Singleton<NumericalCheck>;

            void perform() const;

        private:
            NumericalCheck();
            virtual ~NumericalCheck();
    };

}

#endif
