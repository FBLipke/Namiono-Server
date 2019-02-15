#include <Namiono-Server.h>
#pragma once

class Packet
{
public:
	EXPORT Packet(const ServiceType* serviceType, const char* data, const _SIZET* length);
	EXPORT Packet(Packet& packet, const _SIZET length, DHCP_MSGTYPE msgType = OFFER);

	EXPORT Packet(const _SIZET length, Packet_OPCode opcode);

	EXPORT ~Packet();
	EXPORT _SIZET Write(const void* data, const _SIZET length, const _SIZET position = 0);
	EXPORT void Write(unsigned int data, const _SIZET position = 0);
	EXPORT void Write(unsigned char data, const _SIZET position = 0);
	EXPORT void Write(unsigned short data, const _SIZET position = 0);
	EXPORT _SIZET Read(void* data, const _SIZET length, const _SIZET position);
	EXPORT void Set_Opcode(Packet_OPCode op);
	EXPORT Packet_OPCode Get_Opcode();

	EXPORT char* Get_Buffer();

	EXPORT void set_opcode(const DHCP_OPCODE op);
	EXPORT DHCP_OPCODE get_opcode();

	EXPORT void set_hwtype(const DHCP_HARDWARETYPE hwtype);
	EXPORT DHCP_HARDWARETYPE get_hwtype();

	EXPORT void set_hwlength(const unsigned char length);
	EXPORT unsigned char get_hwlength();

	EXPORT void set_hops(const unsigned char hops);
	EXPORT unsigned char get_hops();

	EXPORT void set_xid(const unsigned int xid);
	EXPORT unsigned int get_xid();

	EXPORT void set_secs(const unsigned short secs);
	EXPORT unsigned short get_secs();

	EXPORT void set_flags(const DHCP_FLAGS flags);
	EXPORT DHCP_FLAGS get_flags();

	EXPORT void set_clientIP(const _IPADDR ip);
	EXPORT _IPADDR get_clientIP();

	EXPORT void set_yourIP(const _IPADDR ip);
	EXPORT _IPADDR get_yourIP();

	EXPORT void set_nextIP(const _IPADDR ip);
	EXPORT _IPADDR get_nextIP();

	EXPORT void set_relayIP(const _IPADDR ip);
	EXPORT _IPADDR get_relayIP();

	EXPORT void set_hwaddress(const void* mac);
	EXPORT void get_hwaddress(char* out, _SIZET length);

	EXPORT void set_servername(const std::string& sname);
	EXPORT std::string get_servername();

	EXPORT void set_filename(const std::string& file);
	EXPORT std::string get_filename();

	EXPORT void set_cookie();
	EXPORT unsigned int get_cookie();

	EXPORT void set_Length(const _SIZET length);
	EXPORT _SIZET get_Length();

	EXPORT DHCP_Option Get_DHCPOption(const unsigned char option);
	EXPORT TFTP_Option Get_TFTPOption(const std::string option);

	EXPORT void Add_DHCPOption(const DHCP_Option option);
	EXPORT void Remove_DHCPOption(const unsigned char& opt);
	EXPORT bool Has_DHCPOption(const unsigned char& option);

	EXPORT void Add_TFTPOption(const TFTP_Option option);
	EXPORT void Remove_TFTPOption(const std::string& opt);
	EXPORT bool Has_TFTPOption(const std::string& option);
	EXPORT void Commit();

	EXPORT void Set_Block(const unsigned short block);
	EXPORT unsigned short Get_Block();
private:
	EXPORT void Trim();

	char* buffer;
	ServiceType serviceType;
	Packet_OPCode opcode;
	_SIZET packetLength = 0;

	std::map<unsigned char, DHCP_Option> dhcp_options;
	std::map<std::string, TFTP_Option> tftp_options;
};

