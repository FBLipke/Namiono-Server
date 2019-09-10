#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		Client::Client(const ServiceType& sType, const std::string& serverid, const sockaddr_in& hint, const std::string& id)
		{
			this->type = sType;

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
					if (this->dhcp->GetIsBSDPRequest())
					{
						if (!this->dhcp->GetIsRelayedPacket())
							this->dhcp->Get_BSDPClient()->Set_ReplyPort(this->Get_Port());
					}
					break;
				}
				break;
			case TFTP_SERVER:
				this->_hint = hint;
				this->tftp = new TFTP_Client();
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
			this->_hint = _hint;
			this->_hint.sin_addr.s_addr = addr;
			this->_hint.sin_port = htons(port);
		}

		void Client::Set_Client_Hint(const sockaddr_in& hint)
		{
			this->_hint = hint;

			if (this->_hint.sin_addr.s_addr == 0)
				this->_hint.sin_addr.s_addr = INADDR_BROADCAST;
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
			this->_hint.sin_port = port;
		}

		_USHORT Client::Get_Port() const
		{
			return this->_hint.sin_port;
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
			return this->_hint;
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

		Client::~Client()
		{
			delete this->tftp;
			this->tftp = nullptr;

			delete this->dhcp;
			this->dhcp = nullptr;

			delete this->response;
			this->response = nullptr;
		}
	}
}