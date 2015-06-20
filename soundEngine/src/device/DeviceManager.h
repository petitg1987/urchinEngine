#ifndef ENGINE_DEVICEMANAGER_H
#define ENGINE_DEVICEMANAGER_H

namespace urchin
{

	class DeviceManager
	{
		public:
			DeviceManager();
			~DeviceManager();

			void initializeDevice();
			void shutdownDevice();
	};

}

#endif
