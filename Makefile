CXX = g++
INC_NAMIONO = -I ./src/ -I ./src/Namiono/ -I ./src/Namiono/Network/ -I ./src/Namiono/Network/Packet/ -I ./src/Namiono/Network/Server/ -I ./src/Namiono/Network/Environment/ -I ./src/Namiono/Defines/ -I ./src/Namiono/Defines/DHCP/ -I ./src/Namiono/Defines/BSDP/ -I ./src/Namiono/Defines/IPXE/ -I ./src/Namiono/Defines/TFTP/ -I ./src/Namiono/Defines/WDS/
INC_NAMIONO += -I ./src/Namiono/Network/Client/ -I ./src/Namiono/Network/Client/DHCP/ -I ./src/Namiono/Network/Client/DHCP/BSDP/ -I ./src/Namiono/Network/Client/DHCP/IPXE/ -I ./src/Namiono/Network/Client/DHCP/RBCP/ -I ./src/Namiono/Network/Client/DHCP/WDS/ -I ./src/Namiono/Network/Client/TFTP/
INC_NAMIONO += -I ./src/Namiono/Network/Services/ -I ./src/Namiono/Network/Services/DHCP/ -I ./src/Namiono/Network/Services/TFTP/

CXXFLAGS = -std=c++14 -fpermissive -pthread 
CXXFLAGS += $(INC_NAMIONO)  

all: envi clean namiono

namiono: build/main.o build/environment.o build/Namiono.o build/Functions.o build/Network.o build/DHCP_Service.o build/ProxyDHCP_Service.o build/DHCP_Functions.o build/TFTP_Service.o build/Packet.o build/Client.o build/DHCP_Client.o build/IPXE_Client.o build/BSDP_Client.o build/RBCP_Client.o build/WDS_Client.o build/TFTP_Client.o build/Interface.o build/Server.o build/Filesystem.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o bin/namiono

envi:
	@mkdir -p build
	@mkdir -p bin

build/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/environment.o: src/Environment/environment.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/Namiono.o: src/Namiono/Namiono.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/Functions.o: src/Namiono/Common/Functions.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/Network.o: src/Namiono/Network/Network.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/TFTP_Service.o: src/Namiono/Services/TFTP/TFTP_Service.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/DHCP_Service.o: src/Namiono/Services/DHCP/DHCP_Service.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/ProxyDHCP_Service.o: src/Namiono/Services/DHCP/ProxyDHCP_Service.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/DHCP_Functions.o: src/Namiono/Services/DHCP/DHCP_Functions.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/Packet.o: src/Namiono/Network/Packet/Packet.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/Client.o: src/Namiono/Network/Client/Client.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/DHCP_Client.o: src/Namiono/Network/Client/DHCP/DHCP_Client.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/BSDP_Client.o: src/Namiono/Network/Client/DHCP/BSDP/BSDP_Client.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/RBCP_Client.o: src/Namiono/Network/Client/DHCP/RBCP/RBCP_Client.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@
	
build/IPXE_Client.o: src/Namiono/Network/Client/DHCP/IPXE/IPXE_Client.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/WDS_Client.o: src/Namiono/Network/Client/DHCP/WDS/WDS_Client.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/TFTP_Client.o: src/Namiono/Network/Client/TFTP/TFTP_Client.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/Server.o: src/Namiono/Network/Server/Server.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/Interface.o: src/Namiono/Network/Server/Interface.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

build/Filesystem.o: src/Namiono/Common/Filesystem.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -rf build/*.o bin/namiono
