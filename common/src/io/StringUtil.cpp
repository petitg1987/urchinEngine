#include <algorithm>
#include <stdexcept>

#include "io/StringUtil.h"

namespace urchin
{

	void StringUtil::replaceAll(std::string &str, const std::string &from, const std::string &to)
	{
	    size_t startPos = 0;
	    while((startPos = str.find(from, startPos)) != std::string::npos)
	    {
	        str.replace(startPos, from.length(), to);
	        startPos += to.length();
	    }
	}

    void StringUtil::replaceLine(std::string &str, const std::string &startLineValue, const std::string &newLineValue)
    {
        size_t startPos = 0;
        if((startPos = str.find(startLineValue)) != std::string::npos)
        {
            size_t endOfLinePos = str.find('\n', startPos);
            if(endOfLinePos == std::string::npos)
            {
                throw std::runtime_error("Impossible to replace '" + startLineValue + "' by '" + newLineValue + "' because EOL not found");
            }

            str.replace(startPos, endOfLinePos - startPos, newLineValue);
        }
    }

	void StringUtil::split(const std::string &str, const char &delimiter, std::vector<std::string> &tokens)
	{
	    std::string::const_iterator start = str.begin();

	    while(start != str.end())
	    {
	    	std::string::const_iterator temp = std::find(start, str.end(), delimiter);
	        tokens.emplace_back(std::string(start, temp));

	        start = temp;
	        if(start != str.end())
            {
                std::advance(start, 1);
            }
	    }
	}

	bool StringUtil::insensitiveEquals(const std::string &a, const std::string &b)
	{
		if (a.size() != b.size())
		{
			return false;
		}

		for (std::size_t i = 0; i < a.size(); ++i)
		{
		    if (tolower(a[i]) != tolower(b[i]))
		    {
		        return false;
		    }
		}

		return true;
	}

}
