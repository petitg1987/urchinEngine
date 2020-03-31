#ifndef URCHINENGINE_ABSTRACTCONTROLLER_H
#define URCHINENGINE_ABSTRACTCONTROLLER_H

#include "UrchinMapHandler.h"

namespace urchin
{

    class AbstractController
    {
        public:
            explicit AbstractController(MapHandler *);

            virtual bool isModified() const;
            virtual void resetModified();

        protected:
            void markModified();

            MapHandler *getMapHandler() const;

        private:
            bool bIsModified;

            MapHandler *mapHandler;
    };

}

#endif
