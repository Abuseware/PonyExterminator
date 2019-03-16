//g++ -pipe -march=native -o main main.cc -lrf24 -lrf24network -lpqxx -lpq

#define RF_CHANNEL 1
#define RF_NODE 0
#define DB_URI "postgresql://pi@localhost/pi"

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>
#include <csignal>

#include <pqxx/pqxx>

#include <RF24Network/RF24Network.h>
#include <RF24/RF24.h>

RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);
RF24Network network(radio);

pqxx::connection dbConnection(DB_URI);

void killHandler(int sig) {
    dbConnection.disconnect();
    exit(sig);
}

int main() {
    signal(SIGTERM, killHandler);

    radio.begin();
    radio.setChannel(RF_CHANNEL);
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MAX);
    radio.setRetries(1, 15);
    network.begin(RF_NODE);

    while(true) {
        float temperature = -1337;
        float voltage = -1337;
        float charge = -1337;

        network.update();

	std::this_thread::sleep_for(std::chrono::nanoseconds(100));

	for(int id = 0; id < 3; id++) {
            RF24NetworkHeader header(1, id + 65);
            float value;
            if(!network.write(header, &value, sizeof(value))){
                std::cerr << "Couldn't send." << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }

        while(network.available()){
            RF24NetworkHeader header;
            float value;

            network.read(header, &value, sizeof(value));
            switch(header.type - 65) {
                case 0:
                    temperature = value;
                case 1:
                    voltage = value;
                case 2:
                    charge = value;
            }
        }

        if((temperature != -1337) && (charge != -1337) && (voltage != -1337)){
            std::cout << "Temperature: " << temperature << ", Charge: " << charge << "% (" << voltage << "V)"
                      << std::endl;
            pqxx::work dbWork(dbConnection);
            dbWork.exec("INSERT INTO grafana.temperatures (node, temperature, charge) VALUES ( " + dbWork.quote("Salon") + ", " + dbWork.quote(temperature) + ", " + dbWork.quote(charge) + ")");
            dbWork.commit();
        }

        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}
