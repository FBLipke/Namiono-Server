/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		Client::Client(const ServiceType& sType, const std::string& serverid, const sockaddr_in& hint, const std::string& id)
		{
			this->type = sType;
			this->_inIF = new _USHORT(0);
			this->_outIF = new _USHORT(0);

			this->Set_Client_Hint(hint);
			this->Set_Relay_Hint(hint);
			this->Set_Server_Hint(hint);
			this->_socketID = serverid;
			this->response = nullptr;
			this->_id = id;

			switch (this->type)
			{
			case DHCP_SERVER:
			case BINL_SERVER:

				this->dhcp = new DHCP_Client(this->_socketID);

				switch (htons(hint.sin_port))
				{
				case 67:
				case 4011:
					this->_server = hint;
					break;
				case 68:
				default:
					this->_client = hint;
					if (this->dhcp->GetIsBSDPRequest())
					{
						if (!this->dhcp->GetIsRelayedPacket())
							this->dhcp->Get_BSDPClient()->Set_ReplyPort(this->Get_Port());
					}
					break;
				}
				break;
			case TFTP_SERVER:
				this->_client = hint;
				this->tftp = new TFTP_Client();
				break;
			case HTTP_SERVER:

				break;
			default:
				break;
			}
		}

		TFTP_Client* Client::Get_TFTP_Client()
		{
			if (this->tftp == nullptr)
				this->tftp = new TFTP_Client();

			return this->tftp;
		}

		DHCP_Client* Client::Get_DHCP_Client()
		{
			if (this->dhcp == nullptr)
				this->dhcp = new DHCP_Client(this->_socketID);

			return this->dhcp;
		}

		void Client::Set_Client_Hint(const _IPADDR& addr, const _USHORT port)
		{
			this->_client.sin_addr.s_addr = addr;
			this->_client.sin_port = htons(port);
		}

		void Client::Set_Client_Hint(const sockaddr_in& hint)
		{
			this->_client = hint;

			if (this->_client.sin_addr.s_addr == 0)
				this->_client.sin_addr.s_addr = INADDR_BROADCAST;
		}

		void Client::Set_Server_Hint(const sockaddr_in& hint)
		{
			this->_server = hint;
		}

		void Client::Set_Relay_Hint(const sockaddr_in& hint)
		{
			this->_relay = hint;
		}

		void Client::Set_Relay_Hint(const _IPADDR& addr, const _USHORT port)
		{
			this->_relay.sin_addr.s_addr = addr;
			this->_relay.sin_port = htons(port);
		}

		void Client::Set_Port(const _USHORT& port)
		{
			switch (this->Get_ServiceType())
			{
			case DHCP_SERVER:
				switch (htons(port))
				{
				case 67:
				case 4011:
					this->_server.sin_port = htons(port);
					break;
				case 68:
				case 69:
					this->_client.sin_port = htons(port);
					break;
				default:
					this->_client.sin_port = htons(port);
					break;
				}
				break;
			case TFTP_SERVER:
				break;
			default:
				break;
			}
			
		}

		_USHORT Client::Get_Port() const
		{
			switch (this->Get_ServiceType())
			{
			case DHCP_SERVER:
			case BINL_SERVER:
				return this->_client.sin_port;
			case TFTP_SERVER:
				return this->_client.sin_port;
			}
		}

		ServiceType Client::Get_ServiceType() const
		{
			return this->type;
		}

		void Client::Set_ServiceType(const ServiceType& type)
		{
			this->type = type;
		}

		sockaddr_in& Client::Get_Client_Hint()
		{
			return this->_client;
		}

		sockaddr_in& Client::Get_Server_Hint()
		{
			return this->_server;
		}

		sockaddr_in& Client::Get_Relay_Hint()
		{
			return this->_relay;
		}

		const std::string& Client::Get_ID() const
		{
			return this->_id;
		}

		void Client::SetIncomingInterface(const _USHORT& index)
		{
			*this->_inIF = index;
		}

		_USHORT Client::GetIncomingInterface() const
		{
			return *this->_inIF;
		}

		void Client::SetOutgoingInterface(const _USHORT& index)
		{
			*this->_outIF = index;
		}

		_USHORT Client::GetOutgoingInterface() const
		{
			return *this->_outIF;
		}

		Client::~Client()
		{
			delete this->tftp;
			this->tftp = nullptr;

			delete this->dhcp;
			this->dhcp = nullptr;

			delete this->_outIF;
			this->_outIF = nullptr;
			
			delete this->_inIF;
			this->_inIF = nullptr;

			delete this->response;
			this->response = nullptr;
		}

		void Client::HeartBeat()
		{
		}
	}
}