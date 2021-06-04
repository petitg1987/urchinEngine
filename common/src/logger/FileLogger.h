#pragma once

#include <string>
#include <limits>

#include <logger/Logger.h>

namespace urchin {

    class FileLogger : public Logger {
        public:
            explicit FileLogger(std::string);

            const std::string& getFilename() const;

            std::string retrieveContent(unsigned long) const override;
            std::string retrieveTarget() const override;
            void purge() const override;
            void archive() const override;

        private:
            void write(const std::string&) override;

            std::string filename;
    };

}
