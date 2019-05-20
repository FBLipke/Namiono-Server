/*
 * Client.h
 *
 *  Created on: 18.02.2019
 *      Author: lipkegu
 */

#ifndef NAMIONO_NETWORK_CLIENT_CLIENT_H_
#define NAMIONO_NETWORK_CLIENT_CLIENT_H_

#include <Namiono/Namiono.h>
namespace Namiono
{
	namespace Network
	{
		template<class S>
		class Client
		{
		public:
			Client(const ServiceType sType, const std::string& serverid, const S& hint, const std::string& id)
			{
				this->type = sType;
				this->_socketID = serverid;
				this->_hint = hint;
				if (this->_hint.sin_addr.s_addr == 0)
					this->_hint.sin_addr.s_addr = INADDR_BROADCAST;

				this->_id = id;
				this->dhcp = std::unique_ptr<DHCP_CLIENT>(new DHCP_CLIENT());
				this->tftp = std::unique_ptr<TFTP_CLIENT>(new TFTP_CLIENT());
			}

			void Set_Hint(const S& hint)
			{
				this->_hint = hint;
				if (this->_hint.sin_addr.s_addr == 0)
					this->_hint.sin_addr.s_addr = INADDR_BROADCAST;
			}

			void Set_Port(const _USHORT& port)
			{
				this->_hint.sin_port = port;
			}

			std::string Get_TypeString() const
			{
				switch (this->type)
				{
					case BINL_SERVER:
						return std::string("BINL");
					case DHCP_SERVER:
						return std::string("DHCP");
					case TFTP_SERVER:
						return std::string("TFTP");
				}
			}

			const S& Get_Hint() const 
			{
				return this->_hint;
			}

			const std::string& Get_ID() const
			{
				return this->_id;
			}

			virtual ~Client()
			{
			}

			std::unique_ptr<TFTP_CLIENT> tftp;
			std::unique_ptr<DHCP_CLIENT> dhcp;
		private:
			S _hint;
			std::string _id;
			std::string _socketID;
			ServiceType type;
		};
	}
}
#endif /* NAMIONO_NETWORK_CLIENT_CLIENT_H_ */
