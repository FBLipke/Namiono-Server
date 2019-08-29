CXX = g++
CXXFLAGS = -std=c++11 -fpermissive -pthread -I ./ -I ./Namiono/ -I ./Namiono/Network/ -I ./Namiono/Network/Client/ -I ./Namiono/Network/Services/ -I ./Namiono/Network/Services/DHCP/ -I ./Namiono/Network/Services/TFTP/ -I ./Namiono/Network/Packet/ -I ./Namiono/Network/Server/ -I ./Namiono/Network/Environment/
all: namiono

namiono: clean main.o environment.o Namiono.o Functions.o Network.o DHCP_Service.o ProxyDHCP_Service.o DHCP_Functions.o TFTP_Service.o Packet.o Interface.o Server.o Filesystem.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) main.o environment.o Namiono.o Functions.o Network.o DHCP_Service.o ProxyDHCP_Service.o DHCP_Functions.o TFTP_Service.o Packet.o Interface.o Server.o Filesystem.o -o namiono

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

environment.o: Environment/environment.cpp
	$(CXX) $(CXXFLAGS) -c Environment/environment.cpp

Namiono.o: Namiono/Namiono.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Namiono.cpp

Functions.o: Namiono/Common/Functions.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Common/Functions.cpp

Network.o: Namiono/Network/Network.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Network/Network.cpp

TFTP_Service.o: Namiono/Services/TFTP_Service.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Services/TFTP_Service.cpp

DHCP_Service.o: Namiono/Services/DHCP/DHCP_Service.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Services/DHCP/DHCP_Service.cpp

ProxyDHCP_Service.o: Namiono/Services/DHCP/ProxyDHCP_Service.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Services/DHCP/ProxyDHCP_Service.cpp

DHCP_Functions.o: Namiono/Services/DHCP/DHCP_Functions.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Services/DHCP/DHCP_Functions.cpp

Packet.o: Namiono/Network/Packet/Packet.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Network/Packet/Packet.cpp

Server.o: Namiono/Network/Server/Server.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Network/Server/Server.cpp

Interface.o: Namiono/Network/Server/Interface.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Network/Server/Interface.cpp

Filesystem.o: Namiono/Common/Filesystem.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Common/Filesystem.cpp

clean:
	rm -rf *.o namiono
