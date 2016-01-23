#ifndef ENGINE_STRINGUTIL_H
#define ENGINE_STRINGUTIL_H

#include <string>
#include <vector>

namespace urchin
{

	class StringUtil
	{
		public:
			static void replaceAll(std::string &, const std::string &, const std::string &);

			static void split(const std::string &, const char &, std::vector<std::string> &);

			static bool insensitiveEquals(const std::string &, const std::string &);

		private:
			StringUtil();
			~StringUtil();
	};

}

#endif
