#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Services
	{
		BootService::BootService(const BootServerType& bsType)
		{
			this->bsType = bsType;

			std::string _dir = "";

			switch (bsType)
			{
			case BootServerType::PXEAPITestServer:
				_dir = "ApiTest";
				break;
			case BootServerType::WindowsNTBootServer:
				_dir = "IntelX86";
				break;
			case BootServerType::PXEBootstrapServer:
				_dir = "Bstrap";
				break;
			default:
				_dir = "Other";
				break;
			}

			this->rootDir = Combine(Namiono::TFTPRootDir, _dir);
			MakePath(this->rootDir);
		}

		BootService::~BootService()
		{
		}
	}
}
