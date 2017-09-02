#include <algorithm>

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

	void StringUtil::split(const std::string &str, const char &delimitor, std::vector<std::string> &tokens)
	{
	    std::string::const_iterator start = str.begin();

	    while(start != str.end())
	    {
	    	std::string::const_iterator temp = std::find(start, str.end(), delimitor);
	        tokens.emplace_back(std::string(start, temp));

	        start = temp;
	        while((start != str.end()) && (*start == delimitor))
	        {
	        	start++;
	        }
	    }
	}

	bool StringUtil::insensitiveEquals(const std::string &a, const std::string &b)
	{
		if (a.size() != b.size())
		{
			return false;
		}

		for (unsigned int i = 0; i < a.size(); ++i)
		{
		        if (tolower(a[i]) != tolower(b[i]))
		        {
		            return false;
		        }
		}

		return true;
	}

}
