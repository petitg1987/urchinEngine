#pragma once

#include <string>
#include <optional>
#include <memory>
#include <vector>
#include <map>

namespace urchin {

    class DataContentLine {
        public:
            explicit DataContentLine(const std::string&, DataContentLine*);

            DataContentLine* getParent() const;
            void addChild(std::unique_ptr<DataContentLine>);

            const std::string& getName() const;
            const std::string& getValue() const;
            const std::map<std::string, std::string>& getAttributes() const;

        private:
            std::string name;
            std::string value;
            std::map<std::string, std::string> attributes;

            DataContentLine* parent;
            std::vector<std::unique_ptr<DataContentLine>> children;
    };

}
