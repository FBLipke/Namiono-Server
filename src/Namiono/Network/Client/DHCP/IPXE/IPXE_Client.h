#include <Namiono/Namiono.h>
#pragma once
namespace Namiono
{
	namespace Network
	{
		class IPXE_Client
		{
		public:
			IPXE_Client();
			~IPXE_Client();
			void Set_Username(const std::string & username);
			void Set_Password(const std::string & password);
			const std::string & Get_Username() const;
			const std::string & Get_Password() const;
		private:
			std::string* username = nullptr;
			std::string* password = nullptr;
		};
	}
}
