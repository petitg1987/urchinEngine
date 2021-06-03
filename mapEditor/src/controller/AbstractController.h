#ifndef URCHINENGINE_ABSTRACTCONTROLLER_H
#define URCHINENGINE_ABSTRACTCONTROLLER_H

#include <UrchinCommon.h>
#include <UrchinMapHandler.h>

namespace urchin {

    class AbstractController : public Observable {
        public:
            explicit AbstractController();

            enum NotificationType {
                CHANGES_DONE
            };

            virtual void setup(MapHandler*);

            virtual bool isModified() const;
            virtual void resetModified();

        protected:
            void markModified();

            MapHandler* getMapHandler() const;

        private:
            bool bIsModified;

            MapHandler* mapHandler;
    };

}

#endif
