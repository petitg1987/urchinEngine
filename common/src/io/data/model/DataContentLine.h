#pragma once

#include <string>
#include <optional>
#include <memory>
#include <vector>
#include <map>

#include <io/data/model/DataContentLine.h>

namespace urchin {

    class DataContentLine {
        public:
            DataContentLine(std::string, std::string, std::map<std::string, std::string>, DataContentLine*);
            static std::unique_ptr<DataContentLine> fromRawContentLine(const std::string&, DataContentLine*, const std::string&);

            DataContentLine* getParent() const;

            void addChild(std::unique_ptr<DataContentLine>);
            const std::vector<std::unique_ptr<DataContentLine>>& getChildren() const;

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
