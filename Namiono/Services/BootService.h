#pragma once

#include <Namiono/Namiono.h>
namespace Namiono
{
	namespace Services
	{
		class BootService
		{
		public:
			BootService(const BootServerType& bsType);
			virtual ~BootService();

		private:
			BootServerType bsType;
			std::string rootDir;
		};
	}
}

