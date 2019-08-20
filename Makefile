CXX = g++
CXXFLAGS = -std=c++11 -fpermissive -pthread -I ./ -I ./Namiono/ -I ./Namiono/Network/ -I ./Namiono/Network/Client/ -I ./Namiono/Network/Packet/ -I ./Namiono/Network/Server/ -I ./Namiono/Network/Environment/
all: namiono

namiono: clean main.o environment.o Namiono.o Functions.o Network.o Packet.o Filesystem.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) main.o environment.o Namiono.o Functions.o Network.o Packet.o Filesystem.o -o namiono

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

environment.o: Environment/environment.cpp
	$(CXX) $(CXXFLAGS) -c Environment/environment.cpp

Namiono.o: Namiono/Namiono.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Namiono.cpp

Functions.o: Namiono/Functions.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Functions.cpp

Network.o: Namiono/Network/Network.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Network/Network.cpp

Packet.o: Namiono/Network/Packet/Packet.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Network/Packet/Packet.cpp

Filesystem.o: Namiono/Filesystem.cpp
	$(CXX) $(CXXFLAGS) -c Namiono/Filesystem.cpp

clean:
	rm -rf *.o namiono