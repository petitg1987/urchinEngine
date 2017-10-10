#ifndef URCHINENGINE_SINGLETON_H
#define URCHINENGINE_SINGLETON_H

#include <typeinfo>
#include <iostream>

#include "pattern/singleton/SingletonManager.h"
#include "pattern/singleton/SingletonInterface.h"

namespace urchin
{

	/**
	* Allows to create a singleton class
	*/
	template<class T> class Singleton : public SingletonInterface
	{
		public:
			static T *instance();
			~Singleton() override;

		protected:
			Singleton();

		private:
			static T *objectT;
	};

	#include "Singleton.inl"

}

#endif
