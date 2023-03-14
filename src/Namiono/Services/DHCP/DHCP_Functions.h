#pragma once
#include <Namiono/Namiono.h>
using namespace Namiono::Network;

class DHCP_Functions
{
public:

	DHCP_Functions();
	~DHCP_Functions();

	static void Create_BootServerList(std::vector<BootServerEntry>* serverlist, Client* client);
	static void Add_BootServer_To_ServerList(std::vector<BootServerEntry>* serverlist,
		Server* server, Client * client, const std::string& serverName, const std::string& bootfile);

	static bool Has_BootServer(std::vector<BootServerEntry>* serverlist, const _USHORT & id);
	static void Add_BootServer(std::vector<BootServerEntry>* serverlist, BootServerType bstype,
		const std::string& name, const std::vector <_IPADDR>& addresses, const std::string& bootfile);

	static void Generate_Bootmenu_From_ServerList(SETTINGS* settings, std::vector<BootServerEntry>* serverlist, Client* client);
	static void Relay_Request_Packet(const _IPADDR& addr, const _USHORT& port, const ServiceType& type, Server * server, _USHORT iface, Client * client);
	static _USHORT Handle_Relayed_Packet(const ServiceType& type, Server * server, _USHORT iface, Packet * packet);

	static void Relay_Response_Packet(std::map<std::string, DHCP_RELAYSESSION>* relaySessions, const ServiceType & type, Server * server, _USHORT iface, Client * client, Packet* packet);
	static void Handle_IPXE_Options(Server * server, _USHORT iface, Client * client, Packet * response);
	static void Handle_WDS_Options(const SETTINGS* settings, const ServiceType& type, Server * server, _USHORT iface, Client * client);
};

