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

#ifndef NAMIONO_NETWORK_CLIENT_CLIENT_H_
#define NAMIONO_NETWORK_CLIENT_CLIENT_H_

#include <Namiono/Namiono.h>
namespace Namiono
{
	namespace Network
	{
		class Client
		{
		public:
			Client(const ServiceType& sType, const std::string& serverid, const sockaddr_in& hint, const std::string& id);
			TFTP_Client* Get_TFTP_Client();
			DHCP_Client* Get_DHCP_Client();

			void Set_Client_Hint(const _IPADDR& addr, const _USHORT port);
			void Set_Client_Hint(const sockaddr_in& hint);
			void Set_Server_Hint(const sockaddr_in& hint);
			void Set_Relay_Hint(const sockaddr_in& hint);
			void Set_Relay_Hint(const _IPADDR& addr, const _USHORT port);
			void Set_Port(const _USHORT& port);
			_USHORT Get_Port() const;
			ServiceType Get_ServiceType() const;
			void Set_ServiceType(const ServiceType& type);

			void SetIncomingInterface(const _USHORT& index);
			_USHORT GetIncomingInterface() const;

			void SetOutgoingInterface(const _USHORT& index);
			_USHORT GetOutgoingInterface() const;

			sockaddr_in& Get_Client_Hint();
			sockaddr_in& Get_Server_Hint();
			sockaddr_in& Get_Relay_Hint();
			const std::string& Get_ID() const;
			virtual ~Client();


			void HeartBeat();

			Packet* response = nullptr;
			std::string _socketID = "";
		private:
			_USHORT* _inIF = nullptr;
			_USHORT* _outIF = nullptr;
			TFTP_Client* tftp = nullptr;
			DHCP_Client* dhcp = nullptr;

			sockaddr_in _client;
			sockaddr_in _relay;
			sockaddr_in _server;

			std::string _id = "";

			ServiceType type;
		};
	}
}
#endif /* NAMIONO_NETWORK_CLIENT_CLIENT_H_ */
