//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "CarLogicManager.h"

Define_Module(CarLogicManager);

CarLogicManager::CarLogicManager() {
    // TODO Auto-generated constructor stub

}

CarLogicManager::~CarLogicManager() {
    // TODO Auto-generated destructor stub
}

void CarLogicManager::initialize(int stage) {
    cSimpleModule::initialize(stage);
    if (stage != 1){
        return;
    }

    hostname = par("hostname").stringValue();
    port = par("port");

    try {
        TCPSocket soc(hostname, port);
        socket = &soc;
    }
    catch(SocketException &e) {
        throw cRuntimeError("FUCK");
        printf("RAWWWWW!");
        cerr << e.what() << endl;
        exit(1);
    }

    socket->connect(hostname, port);

    char initPacket[] = "Hello";
    int initPacketLength = strlen(initPacket);
    socket->send(initPacket, initPacketLength);

    updateInterval = par("updateInterval");
    executeOneTimestepTrigger = new cMessage("step");
    scheduleAt(updateInterval, executeOneTimestepTrigger);

}

void CarLogicManager::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        handleSelfMsg(msg);
        return;
    }
    //must be message to be sent to python
    send(check_and_cast<Send *>(msg));
}

void CarLogicManager::handleSelfMsg(cMessage *msg) {
    if (msg == executeOneTimestepTrigger) {
        executeOneTimestep();
        scheduleAt(simTime()+updateInterval, executeOneTimestepTrigger);
    }
    else
        error("CarLogicManager received unknown self-message");
}

void CarLogicManager::executeOneTimestep() {
    unsigned int buffer_len = 1024;
    char buffer[buffer_len+1];
    int bytesReceived;
    if ((bytesReceived = (socket->recv(buffer, buffer_len))) <= 0) return;
    if (buffer == NULL) return;
    buffer[bytesReceived] = '\0';
    printf("buffer:\n%s", buffer);
}

void CarLogicManager::send(Send *msg) {
    const char *buffer = msg->getData();
    unsigned int buffer_len = strlen(buffer);
    socket->send(buffer, buffer_len);
}

void CarLogicManager::finish() {
    //nothing yet
}
