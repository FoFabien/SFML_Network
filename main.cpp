#include <iostream>
#include <ctime>

#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include "mlib.hpp"
#include "network.hpp"


NetworkServer *network = NULL;
bool terminate = false;
void client_test()
{
    NetworkClient clients[5];
    srand(time(0));

    //if(!network->selfConnection(sf::Socket::AnyPort)) std::cout << "Client 0 connected\n";
    //else std::cout << "Client 0 connected\n";

    for(int i = 0; i < 4; ++i)
    {
        sf::sleep(sf::milliseconds(500));
        if(!clients[i].connect(sf::IpAddress::LocalHost, 54321, sf::Socket::AnyPort)) std::cout << "Client " << i/*+1*/ << " connected with ID #" << (char)(clients[i].getID()+'0') << "\n";
        else std::cout << "Client " << i/*+1*/ << " failed to connect\n";
        //if(i <= 2) clients[i].disconnect();
    }
    std::cout << "TestClient: Sending Data with clients...\n";
    for(int i = 0; i < 4; ++i)
    {
        sf::Packet p;
        p << clients[i].getID();
        p << rand();
        if(!clients[i].send(p)) std::cout << "Client " << i/*+1*/ << " failed to send data\n";
    }
    sf::sleep(sf::milliseconds(1000));
    terminate = true;
}

void server_test()
{
    std::vector<uint8_t> ids;
    while(!terminate)
    {
        network->update();
        if(!network->receive()) std::cout << "Failed to receive" << std::endl;
        ids = network->getConnectedID();
        for(uint8_t &i : ids)
        {
            if(network->hasDataFromClient(i))
            {
                sf::Packet p;
                uint8_t test1;
                int test2;
                p = network->getPacketFromClient(i);
                p >> test1 >> test2;
                std::cout << "TestServer: Data received: " << (char)(test1+'0') << " " << test2 << std::endl;
            }
            else std::cout << "TestServer: No Data from client #" << (int32_t)i << std::endl;
        }
    }
}

int main()
{
    Out.setOutput(true, false);
    Out.useTimestamp(false);

    sf::Thread *t1 = new sf::Thread(&client_test);
    sf::Thread *t2 = new sf::Thread(&server_test);
    network = new NetworkServer();

    network->start(54321, sf::Socket::AnyPort, false);
    Out = "*START*\n";
    t1->launch();
    t2->launch();
    while(!terminate)
    {
        network->listen();
    }
    sf::sleep(sf::seconds(1));
    network->stop();
    Out = "*STOP*\n";
    t1->wait();
    t2->wait();

    delete t1;
    delete t2;
    delete network;
	return 0;
}
