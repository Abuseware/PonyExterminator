//g++ -Wall -Wextra -pipe -Ofast -march=native -o main main.cc -lrf24 -lrf24network -lpqxx -lpq

#define RF_CHANNEL 1
#define RF_NODE 0
#define DB_URI "postgresql://user:pass@host/db"

#define MAGIC -31337

#include <array>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>
#include <csignal>

#include <pqxx/pqxx>

#include <RF24Network/RF24Network.h>
#include <RF24/RF24.h>

//RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ); // <-- This is for native RPi driver, needs root rights to work! 
RF24 radio(22,0); // <-- This is for Linux /dev/spidev0.0
RF24Network network(radio);

pqxx::connection dbConnection(DB_URI);

struct __attribute__((packed)) packet_t {
  float temperature;
  float voltage;
  float charge;
};

std::array<std::string, 5> nodeMap = {
    "Node 1",
    "Node 2",
    "Node 3",
    "Node 4",
    "Node 5"
};

void killHandler(int sig) {
    std::cout << "Disconnect from database..." << std::endl;
    dbConnection.disconnect();
    std::cout << "Exiting..." << std::endl;
    exit(sig);
}

int main() {
    signal(SIGTERM, killHandler);

    dbConnection.prepare("ins", "INSERT INTO grafana.temperatures (node, temperature, charge) VALUES ( $1, $2, $3)");

    radio.begin();
    radio.setChannel(RF_CHANNEL);
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MAX);
    radio.setRetries(1, 15);
    network.begin(RF_NODE);
    std::cout << "Ready!" << std::endl;

    while(true) {
        if(!dbConnection.is_open()){
            std::cerr << "Disconnected from database, exiting." << std::endl;
            return 1;
        }

        network.update();

        pqxx::work dbWork(dbConnection);
        
        while(network.available()){
            RF24NetworkHeader header;
            packet_t packet;

            network.peek(header);
            switch(header.type) {
                case 0:
                    network.read(header, nullptr, 0);
                    std::cout << "[" << nodeMap[header.from_node - 1] << "] online!" << std::endl;
                    break;
                case 65:
                    network.read(header, &packet, sizeof(packet));

                    std::cout << "[" << nodeMap[header.from_node - 1] << "] "
                              << "Temperature: " << packet.temperature << "°C"
                              << ", Charge: " << packet.charge << "%"
                              << " (" << packet.voltage << "V)"
                              << std::endl;

            
                    dbWork.prepared("ins")(nodeMap[header.from_node - 1])(packet.temperature)(packet.charge).exec();
                    break;
                default:
                    std::cerr << "Received bad packet from " << nodeMap[header.from_node - 1] << ": type = " << header.type << std::endl;
                    break;    
            }
        }
        
        dbWork.commit();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    killHandler(SIGTERM);
}
