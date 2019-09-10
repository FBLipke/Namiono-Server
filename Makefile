CXX = g++
INC_NAMIONO = -I ./src/ -I ./src/Namiono/ -I ./src/Namiono/Network/ -I ./src/Namiono/Network/Packet/ -I ./src/Namiono/Network/Server/ -I ./src/Namiono/Network/Environment/ -I ./src/Namiono/Defines/ -I ./src/Namiono/Defines/DHCP/ -I ./src/Namiono/Defines/BSDP/ -I ./src/Namiono/Defines/IPXE/ -I ./src/Namiono/Defines/TFTP/ -I ./src/Namiono/Defines/WDS/
INC_NAMIONO += -I ./src/Namiono/Network/Client/ -I ./src/Namiono/Network/Client/DHCP/ -I ./src/Namiono/Network/Client/DHCP/BSDP/ -I ./src/Namiono/Network/Client/DHCP/IPXE/ -I ./src/Namiono/Network/Client/DHCP/RBCP/ -I ./src/Namiono/Network/Client/DHCP/WDS/ -I ./src/Namiono/Network/Client/TFTP/
INC_NAMIONO += -I ./src/Namiono/Network/Services/ -I ./src/Namiono/Network/Services/DHCP/ -I ./src/Namiono/Network/Services/TFTP/

CXXFLAGS = -std=c++11 -fpermissive -pthread 
CXXFLAGS += $(INC_NAMIONO)  

all: namiono

namiono: clean main.o environment.o Namiono.o Functions.o Network.o DHCP_Service.o ProxyDHCP_Service.o DHCP_Functions.o TFTP_Service.o Packet.o Client.o DHCP_Client.o IPXE_Client.o BSDP_Client.o RBCP_Client.o WDS_Client.o TFTP_Client.o Interface.o Server.o Filesystem.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) main.o environment.o Namiono.o Functions.o Network.o DHCP_Service.o ProxyDHCP_Service.o DHCP_Functions.o TFTP_Service.o Packet.o Client.o DHCP_Client.o IPXE_Client.o BSDP_Client.o RBCP_Client.o WDS_Client.o TFTP_Client.o Interface.o Server.o Filesystem.o -o namiono

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp

environment.o: src/Environment/environment.cpp
	$(CXX) $(CXXFLAGS) -c src/Environment/environment.cpp

Namiono.o: src/Namiono/Namiono.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Namiono.cpp

Functions.o: src/Namiono/Common/Functions.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Common/Functions.cpp

Network.o: src/Namiono/Network/Network.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Network.cpp

TFTP_Service.o: src/Namiono/Services/TFTP/TFTP_Service.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Services/TFTP/TFTP_Service.cpp

DHCP_Service.o: src/Namiono/Services/DHCP/DHCP_Service.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Services/DHCP/DHCP_Service.cpp

ProxyDHCP_Service.o: src/Namiono/Services/DHCP/ProxyDHCP_Service.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Services/DHCP/ProxyDHCP_Service.cpp

DHCP_Functions.o: src/Namiono/Services/DHCP/DHCP_Functions.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Services/DHCP/DHCP_Functions.cpp

Packet.o: src/Namiono/Network/Packet/Packet.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Packet/Packet.cpp

Client.o: src/Namiono/Network/Client/Client.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Client/Client.cpp

DHCP_Client.o: src/Namiono/Network/Client/DHCP/DHCP_Client.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Client/DHCP/DHCP_Client.cpp

BSDP_Client.o: src/Namiono/Network/Client/DHCP/BSDP/BSDP_Client.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Client/DHCP/BSDP/BSDP_Client.cpp

RBCP_Client.o: src/Namiono/Network/Client/DHCP/RBCP/RBCP_Client.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Client/DHCP/RBCP/RBCP_Client.cpp
	
IPXE_Client.o: src/Namiono/Network/Client/DHCP/IPXE/IPXE_Client.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Client/DHCP/IPXE/IPXE_Client.cpp

WDS_Client.o: src/Namiono/Network/Client/DHCP/WDS/WDS_Client.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Client/DHCP/WDS/WDS_Client.cpp

TFTP_Client.o: src/Namiono/Network/Client/TFTP/TFTP_Client.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Client/TFTP/TFTP_Client.cpp

Server.o: src/Namiono/Network/Server/Server.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Server/Server.cpp

Interface.o: src/Namiono/Network/Server/Interface.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Server/Interface.cpp

Filesystem.o: src/Namiono/Common/Filesystem.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Common/Filesystem.cpp

clean:
	rm -rf *.o namiono
