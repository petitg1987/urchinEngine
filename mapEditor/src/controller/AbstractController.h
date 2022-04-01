#pragma once

#include <UrchinCommon.h>
#include <UrchinAggregation.h>

namespace urchin {

    class AbstractController : public Observable {
        public:
            explicit AbstractController();
            ~AbstractController() override = default;

            enum NotificationType {
                CHANGES_DONE
            };

            virtual void setup(Map&);

            virtual bool isModified() const;
            virtual void resetModified();

        protected:
            void markModified();

            Map& getMap() const;

        private:
            bool bIsModified;

            Map* map;
    };

}
