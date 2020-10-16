#include "FileReaderUtil.h"

namespace urchin {
    void FileReaderUtil::nextLine(std::ifstream &file, std::string &buffer) {
        do
        {
            std::getline(file, buffer);

            //delete '\r'
            if(buffer.length() > 0 && buffer[buffer.length() - 1] == '\r') {
                buffer.resize(buffer.length() - 1);
            }
        }while(buffer.length()==0 && !file.eof());
    }
}
