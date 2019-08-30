CXX = g++
CXXFLAGS = -std=c++11 -fpermissive -pthread -I ./src/ -I ./src/Namiono/ -I ./src/Namiono/Network/ -I ./src/Namiono/Network/Client/ -I ./src/Namiono/Network/Services/ -I ./src/Namiono/Network/Services/DHCP/ -I ./src/Namiono/Network/Services/TFTP/ -I ./src/Namiono/Network/Packet/ -I ./src/Namiono/Network/Server/ -I ./src/Namiono/Network/Environment/
all: namiono

namiono: clean main.o environment.o Namiono.o Functions.o Network.o DHCP_Service.o ProxyDHCP_Service.o DHCP_Functions.o TFTP_Service.o Packet.o Interface.o Server.o Filesystem.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) main.o environment.o Namiono.o Functions.o Network.o DHCP_Service.o ProxyDHCP_Service.o DHCP_Functions.o TFTP_Service.o Packet.o Interface.o Server.o Filesystem.o -o namiono

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

Server.o: src/Namiono/Network/Server/Server.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Server/Server.cpp

Interface.o: src/Namiono/Network/Server/Interface.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Network/Server/Interface.cpp

Filesystem.o: src/Namiono/Common/Filesystem.cpp
	$(CXX) $(CXXFLAGS) -c src/Namiono/Common/Filesystem.cpp

clean:
	rm -rf *.o namiono
