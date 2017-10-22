#include "FileReaderUtil.h"

namespace urchin
{
    void FileReaderUtil::nextLine(std::ifstream &file, std::string &buffer)
    {
        do
        {
            std::getline(file, buffer);

            //delete '\r'
            unsigned long length = (buffer.length() > 0) ? buffer.length()-1 : 0;
            if(length >= 0 && buffer[length] == '\r')
            {
                buffer.resize(length);
            }
        }while(buffer.length()==0 && !file.eof());
    }
}
