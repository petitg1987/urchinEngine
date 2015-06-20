#ifndef ENGINE_RESOURCE_H
#define ENGINE_RESOURCE_H

#include <string>

namespace urchin
{
	
	class Resource
	{
		public:
			Resource();
			virtual ~Resource();
		
			const std::string& getName() const;
			void setName(const std::string &name);
			void addRef();
			void release();

		private:
			std::string name;
			unsigned int refCount;
	};

}

#endif
