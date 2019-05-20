/*
 * Packet.h
 *
 *  Created on: 18.02.2019
 *      Author: lipkegu
 */

#ifndef NAMIONO_NETWORK_PACKET_H_
#define NAMIONO_NETWORK_PACKET_H_
#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		class Packet
		{
		public:
			Packet(const ServiceType& serviceType, const char* data, const _SIZET* length);
			Packet(Packet& packet, const _SIZET length, const DHCP_MSGTYPE& msgType);
			Packet() {};
			Packet(const _SIZET& length, const Packet_OPCode& opcode);

			~Packet();
			_SIZET Write(const void* data, const _SIZET length, const _SIZET position = 0);
			void Write(_ULONG data, const _SIZET position = 0);
			void Write(_UINT data, const _SIZET position = 0);

			void Write(_BYTE data, const _SIZET position = 0);
			void Write(_USHORT data, const _SIZET position = 0);
			_SIZET Read(void* data, const _SIZET length, const _SIZET position);
			void Set_Opcode(const Packet_OPCode& op);
			const Packet_OPCode& Get_Opcode() const;

			char* Get_Buffer() const;

			void set_opcode(const DHCP_OPCODE op);
			DHCP_OPCODE get_opcode();

			void set_hwtype(const DHCP_HARDWARETYPE hwtype);
			DHCP_HARDWARETYPE get_hwtype();

			void set_hwlength(const _BYTE length);
			_BYTE get_hwlength();

			void set_hops(const _BYTE hops);
			_BYTE get_hops();

			void set_xid(const _ULONG xid);
			_ULONG get_xid();

			void set_secs(const _USHORT secs);
			_USHORT get_secs();

			void set_flags(const DHCP_FLAGS flags);
			DHCP_FLAGS get_flags();

			void set_clientIP(const _IPADDR ip);
			_IPADDR get_clientIP();

			void set_yourIP(const _IPADDR ip);
			_IPADDR get_yourIP();

			void set_nextIP(const _IPADDR ip);
			_IPADDR get_nextIP();

			void set_relayIP(const _IPADDR ip);
			_IPADDR get_relayIP();

			void set_hwaddress(const void* mac);
			void get_hwaddress(char* out, _SIZET length);

			void set_servername(const std::string& sname);
			std::string get_servername();

			void set_filename(const std::string& file);
			std::string get_filename();

			void set_cookie();
			_ULONG get_cookie();

			void set_Length(const _SIZET length);
			_SIZET get_Length() const;

			DHCP_Option Get_DHCPOption(const _BYTE& option) const;
			TFTP_Option Get_TFTPOption(const std::string& option) const;

			void Add_DHCPOption(const DHCP_Option option);
			void Remove_DHCPOption(const _BYTE& opt);
			bool Has_DHCPOption(const _BYTE& option);

			void Add_TFTPOption(const TFTP_Option option);
			void Remove_TFTPOption(const std::string& opt);
			bool Has_TFTPOption(const std::string& option);
			void Commit();

			void Set_Block(const _USHORT block);
			_USHORT Get_Block();
		private:
			void Trim();

			char* buffer;
			ServiceType serviceType;
			Packet_OPCode opcode;
			_SIZET packetLength = 0;

			std::map<_BYTE, DHCP_Option> dhcp_options;
			std::map<std::string, TFTP_Option> tftp_options;
		};
	} /* namespace Network */
} /* namespace Namiono */

#endif /* NAMIONO_NETWORK_PACKET_H_ */
