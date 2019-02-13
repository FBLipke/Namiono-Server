#include <Namiono-Server.h>
#pragma once

class Packet
{
public:
	EXPORT Packet(const ServiceType* serviceType, const char* data, const _SIZE_T* length);
	EXPORT Packet(Packet& packet, const _SIZE_T length, DHCP_MSGTYPE msgType = OFFER);

	EXPORT Packet(const _SIZE_T length, Packet_OPCode opcode);

	EXPORT ~Packet();
	EXPORT _SIZE_T Write(const void* data, const _SIZE_T length, const _SIZE_T position = 0);
	EXPORT void Write(unsigned int data, const _SIZE_T position = 0);
	EXPORT void Write(unsigned char data, const _SIZE_T position = 0);
	EXPORT void Write(unsigned short data, const _SIZE_T position = 0);
	EXPORT _SIZE_T Read(void* data, const _SIZE_T length, const _SIZE_T position);
	EXPORT void Set_Opcode(Packet_OPCode op);
	EXPORT INLINE Packet_OPCode Get_Opcode();

	EXPORT INLINE char* Get_Buffer();

	EXPORT INLINE void set_opcode(const DHCP_OPCODE op);
	EXPORT INLINE DHCP_OPCODE get_opcode();

	EXPORT INLINE void set_hwtype(const DHCP_HARDWARETYPE hwtype);
	EXPORT INLINE DHCP_HARDWARETYPE get_hwtype();

	EXPORT INLINE void set_hwlength(const unsigned char length);
	EXPORT INLINE unsigned char get_hwlength();

	EXPORT INLINE void set_hops(const unsigned char hops);
	EXPORT INLINE unsigned char get_hops();

	EXPORT INLINE void set_xid(const unsigned int xid);
	EXPORT INLINE unsigned int get_xid();

	EXPORT INLINE void set_secs(const unsigned short secs);
	EXPORT INLINE unsigned short get_secs();

	EXPORT INLINE void set_flags(const DHCP_FLAGS flags);
	EXPORT INLINE DHCP_FLAGS get_flags();

	EXPORT INLINE void set_clientIP(const _IPADDR ip);
	EXPORT INLINE _IPADDR get_clientIP();

	EXPORT INLINE void set_yourIP(const _IPADDR ip);
	EXPORT INLINE _IPADDR get_yourIP();

	EXPORT INLINE void set_nextIP(const _IPADDR ip);
	EXPORT INLINE _IPADDR get_nextIP();

	EXPORT INLINE void set_relayIP(const _IPADDR ip);
	EXPORT INLINE _IPADDR get_relayIP();

	EXPORT INLINE void set_hwaddress(const void* mac);
	EXPORT INLINE void get_hwaddress(char* out, _SIZE_T length);

	EXPORT INLINE void set_servername(const std::string& sname);
	EXPORT INLINE std::string get_servername();

	EXPORT INLINE void set_filename(const std::string& file);
	EXPORT INLINE std::string get_filename();

	EXPORT INLINE void set_cookie();
	EXPORT INLINE unsigned int get_cookie();

	EXPORT INLINE void set_Length(const _SIZE_T length);
	EXPORT INLINE _SIZE_T get_Length();

	EXPORT INLINE DHCP_Option Get_DHCPOption(const unsigned char option);
	EXPORT INLINE TFTP_Option Get_TFTPOption(const std::string option);

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
	_SIZE_T packetLength = 0;

	std::map<unsigned char, DHCP_Option> dhcp_options;
	std::map<std::string, TFTP_Option> tftp_options;
};

