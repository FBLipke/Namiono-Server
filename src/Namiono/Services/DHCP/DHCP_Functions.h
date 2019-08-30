#pragma once
#include <Namiono/Namiono.h>
using namespace Namiono::Network;

class DHCP_Functions
{
public:

	DHCP_Functions();
	~DHCP_Functions();

	static void Create_BootServerList(std::vector<BootServerEntry>* serverlist, Client* client);
	static void Add_BootServer_To_ServerList(std::vector<BootServerEntry>* serverlist, Server* server, Client * client, const std::string& serverName);
	static bool Has_BootServer(std::vector<BootServerEntry>* serverlist, const _USHORT & id);
	static void Add_BootServer(std::vector<BootServerEntry>* serverlist, const std::string & name, const std::vector<_IPADDR>& addresses);
	static void Generate_Bootmenu_From_ServerList(std::vector<BootServerEntry>* serverlist, Client* client);
	static void Relay_Request_Packet(const _IPADDR& addr, const _USHORT& port, const ServiceType& type, Server * server, int iface, Client * client);
	static _INT32 Handle_Relayed_Packet(Server * server, int iface, Packet * packet);

	static void Relay_Response_Packet(const ServiceType & type, Server * server, int iface, Client * client);
	static void Handle_IPXE_Options(Server * server, int iface, Client * client, Packet * response);
	static void Handle_WDS_Options(Server * server, int iface, Client * client);
};

